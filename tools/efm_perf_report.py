#!/usr/bin/env python3
"""Read Digidrum EFM ColdFire timing counters through digihealth USB PEEK.

No emulator timing is used. The report can also decode a saved SysEx reply or
raw 640-byte memory dump. Live USB mode needs mido and python-rtmidi.
"""
import argparse
import json
import struct
import sys
import time
from pathlib import Path

HEADER = bytes.fromhex('f0 00 20 3c 7d 00')
KINDS = ('BD', 'SD', 'XT', 'CP', 'RS', 'CB', 'HH', 'CY')
MODES = ('idle', 'steady', 'trigger', 'control')
SLOT_BYTES = 20
TABLE_BYTES = len(KINDS) * len(MODES) * SLOT_BYTES


def symbol_address(path, symbol):
    entry = json.loads(Path(path).read_text())[symbol]
    return int(entry, 16) if isinstance(entry, str) else int(entry)


def pack7(data):
    result = bytearray()
    for start in range(0, len(data), 7):
        group = data[start:start + 7]
        high = sum(((byte >> 7) & 1) << (6 - i) for i, byte in enumerate(group))
        result.append(high)
        result.extend(byte & 127 for byte in group)
    return bytes(result)


def unpack7(data):
    result = bytearray()
    for start in range(0, len(data), 8):
        group = data[start:start + 8]
        high = group[0]
        for i, byte in enumerate(group[1:]):
            result.append(byte | (((high >> (6 - i)) & 1) << 7))
    return bytes(result)


def request(seq, command, args=b''):
    return HEADER + pack7(struct.pack('>HB', seq, command) + args) + b'\xf7'


def decode_reply(message, seq, command):
    if not message.startswith(HEADER) or not message.endswith(b'\xf7'):
        raise ValueError('not a digihealth USB SysEx reply')
    body = unpack7(message[len(HEADER):-1])
    if len(body) < 4:
        raise ValueError('short reply')
    got_seq, got_command, status = struct.unpack_from('>HBB', body)
    if got_seq != seq or got_command != (command | 0x80):
        raise ValueError('unexpected sequence or command')
    if status:
        raise ValueError(f'digihealth refused request (status {status})')
    return body[4:]


def live_exchange(out_port, in_port, message, seq, command):
    import mido  # optional: only required when querying hardware
    with mido.open_input(in_port) as input_port, mido.open_output(out_port) as output_port:
        output_port.send(mido.Message.from_bytes(list(message)))
        deadline = time.monotonic() + 5.0
        while time.monotonic() < deadline:
            for response in input_port.iter_pending():
                try:
                    return decode_reply(bytes(response.bytes()), seq, command)
                except ValueError:
                    continue
            time.sleep(0.01)
    raise TimeoutError('no matching digihealth USB reply within five seconds')


def parse_table(data):
    if len(data) != TABLE_BYTES:
        raise ValueError(f'expected {TABLE_BYTES} counter bytes, got {len(data)}')
    return [[struct.unpack_from('>IIIII', data, (k * 4 + m) * SLOT_BYTES)
             for m in range(4)] for k in range(8)]


def format_cell(slot, period):
    seen, count, total, _minimum, maximum = slot
    if not seen or not count:
        return '—'
    average = total / count
    if period:
        return f'{average:.0f} ({100 * average / period:.1f}%)'
    return f'{average:.0f}'


def report(table, period=None, overhead=None, health=None):
    print('EFM render cost per 32-sample call (timer ticks' +
          (', % of full audio-block period' if period else '') + ')')
    if health:
        print(f"Whole render: {health['render_avg']:.1f}% average, "
              f"{health['render_peak']:.1f}% peak; "
              f"CPU busy: {health['cpu_busy']:.1f}% (digihealth window).")
    if overhead is not None:
        print(f'Adjacent timer-read calibration: {overhead} ticks; already subtracted in firmware.')
    print('| Machine | Idle avg | Steady avg | Trigger avg | Control avg | Trigger peak | Trigger samples |')
    print('| --- | ---: | ---: | ---: | ---: | ---: | ---: |')
    for kind, slots in zip(KINDS, table):
        trigger = slots[2]
        peak = (f'{trigger[4]} ({100 * trigger[4] / period:.1f}%)'
                if period and trigger[0] else str(trigger[4]) if trigger[0] else '—')
        print(f'| EFM-{kind} | ' + ' | '.join(format_cell(slot, period) for slot in slots) +
              f' | {peak} | {trigger[0]} |')
    print('Averages use a rolling 512–1024-call window; peaks reset at the window boundary.')
    print('Trigger and control columns need hits or parameter changes on the hardware to populate.')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--map', default='out/Digitakt_OS1.53_DIGIDRUM_EFM_PERF.syx.map.json')
    source = parser.add_mutually_exclusive_group(required=True)
    source.add_argument('--port', help='CoreMIDI port for live USB measurement')
    source.add_argument('--reply', type=Path, help='saved digihealth PEEK SysEx reply')
    source.add_argument('--raw', type=Path, help='raw 640-byte counter dump')
    parser.add_argument('--input-port', help='input port if its name differs from --port')
    args = parser.parse_args()
    address = symbol_address(args.map, 'dp_efm_perf')
    if args.port:
        payload = live_exchange(args.port, args.input_port or args.port,
                                request(1, 3, struct.pack('>IH', address, TABLE_BYTES)), 1, 3)
        raw = payload[4:]
        if len(payload) != TABLE_BYTES + 4 or struct.unpack_from('>I', payload)[0] != address:
            raise ValueError('PEEK replied with the wrong address or length')
        stats = live_exchange(args.port, args.input_port or args.port, request(2, 2), 2, 2)
        window, render_ticks, render_max, renders, idle, idle_render = \
            struct.unpack_from('>IIIIII', stats, 8) if len(stats) >= 32 else (0,) * 6
        period = window / renders if window and renders else None
        health = ({'render_avg': 100 * render_ticks / window,
                   'render_peak': 100 * render_max / period,
                   'cpu_busy': 100 * (1 - max(0, idle - idle_render) / window)}
                  if period else None)
        probe_address = symbol_address(args.map, 'dp_efm_perf_probe_ticks')
        probe = live_exchange(args.port, args.input_port or args.port,
                              request(3, 3, struct.pack('>IH', probe_address, 4)), 3, 3)
        overhead = struct.unpack_from('>I', probe, 4)[0] if len(probe) == 8 else None
    elif args.reply:
        payload = decode_reply(args.reply.read_bytes(), 1, 3)
        if len(payload) != TABLE_BYTES + 4 or struct.unpack_from('>I', payload)[0] != address:
            raise ValueError('PEEK replied with the wrong address or length')
        raw, period, overhead, health = payload[4:], None, None, None
    else:
        raw, period, overhead, health = args.raw.read_bytes(), None, None, None
    report(parse_table(raw), period, overhead, health)


if __name__ == '__main__':
    try:
        main()
    except (OSError, ValueError, TimeoutError, ImportError) as exc:
        sys.exit(f'EFM benchmark: {exc}')
