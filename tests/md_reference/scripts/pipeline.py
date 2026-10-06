#!/usr/bin/env python3
"""TRX-B2 reference/candidate manifest, rendering, measurement and comparison."""
import argparse
import json
import math
import pathlib
import tempfile
import os
import signal
import time
import struct
import subprocess
import wave

ROOT = pathlib.Path(__file__).resolve().parents[3]
MACHINE = ROOT / 'tests/md_reference/machines/trx_b2'
MANIFEST = json.loads((MACHINE / 'machine.json').read_text())


def cases():
    base = MANIFEST['default_patch']
    yield 'baseline', base
    for index, name in enumerate(MANIFEST['parameters']):
        for value in MANIFEST['sweep_values']:
            patch = base.copy()
            patch[index] = value
            yield f'{name.lower()}_{value:03}', patch
    for first, second in MANIFEST['interactions']:
        a = MANIFEST['parameters'].index(first)
        b = MANIFEST['parameters'].index(second)
        for x in MANIFEST['interaction_values']:
            for y in MANIFEST['interaction_values']:
                patch = base.copy()
                patch[a], patch[b] = x, y
                yield f'{first.lower()}_{x:03}__{second.lower()}_{y:03}', patch
    for name in MANIFEST['parameters']:
        yield f'anchor_{name.lower()}', base.copy()
    for first, second in MANIFEST['interactions']:
        yield f'anchor_{first.lower()}_{second.lower()}', base.copy()


def duration_seconds(patch):
    dec = max(0, (patch[1] - 64) / 63)
    hold = patch[3] / 127
    return math.ceil(2 + 28 * dec * dec + 14 * hold * hold)


def groups():
    yield 'baseline', [(name, patch) for name, patch in cases() if name == 'baseline']
    all_cases = list(cases())
    for parameter in MANIFEST['parameters']:
        prefix = parameter.lower() + '_'
        group = parameter.lower()
        yield group, [(f'anchor_{group}', MANIFEST['default_patch'].copy())] + [
            (name, patch) for name, patch in all_cases
            if name.startswith(prefix) and '__' not in name]
    for first, second in MANIFEST['interactions']:
        prefix = first.lower() + '_'
        suffix = '__' + second.lower() + '_'
        group = first.lower() + '_' + second.lower()
        yield group, [(f'anchor_{group}', MANIFEST['default_patch'].copy())] + [
            (name, patch) for name, patch in all_cases
            if name.startswith(prefix) and suffix in name]

def audio(path):
    with wave.open(str(path), 'rb') as wav:
        if wav.getnchannels() != 1 or wav.getsampwidth() not in (2, 3) or wav.getframerate() != 48000:
            raise ValueError(f'{path}: expected mono, 16/24-bit, 48 kHz PCM')
        width = wav.getsampwidth()
        data = wav.readframes(wav.getnframes())
    if width == 2:
        return struct.unpack('<' + 'h' * (len(data) // 2), data)
    return tuple(int.from_bytes(data[i:i+3], 'little', signed=True) / 256
                 for i in range(0, len(data), 3))


def rms(data):
    return math.sqrt(sum(x * x for x in data) / max(1, len(data))) / 32768


def pitch(data, rate=48000):
    # Positive zero crossings on a body-dominated window. Noise-heavy patches
    # may yield an unreliable estimate; callers must inspect voiced confidence.
    crossing = [i for i in range(1, len(data)) if data[i - 1] <= 0 < data[i]]
    intervals = [b - a for a, b in zip(crossing, crossing[1:]) if b > a]
    if len(intervals) < 3:
        return None
    intervals.sort()
    median = intervals[len(intervals) // 2]
    hz = rate / median
    return round(hz, 2) if 20 <= hz <= 2000 else None


def spectrum(data):
    """Windowed 4096-point FFT, enough for broad spectral comparisons."""
    size = 4096
    values = [complex((data[i] / 32768) *
              (0.5 - 0.5 * math.cos(2 * math.pi * i / (size - 1))))
              for i in range(min(len(data), size))]
    values.extend([0j] * (size - len(values)))
    j = 0
    for i in range(1, size):
        bit = size >> 1
        while j & bit:
            j ^= bit
            bit >>= 1
        j ^= bit
        if i < j:
            values[i], values[j] = values[j], values[i]
    length = 2
    while length <= size:
        step = complex(math.cos(-2 * math.pi / length), math.sin(-2 * math.pi / length))
        for start in range(0, size, length):
            factor = 1+0j
            half = length // 2
            for k in range(half):
                u = values[start+k]
                v = factor * values[start+k+half]
                values[start+k] = u+v
                values[start+k+half] = u-v
                factor *= step
        length *= 2
    return [abs(x) for x in values[:size // 2]]


def spectral_metrics(data):
    bins = spectrum(data)
    power = [x*x for x in bins]
    total = sum(power)
    if total < 1e-12:
        return {'centroid_hz': None, 'rolloff_85_hz': None,
                'flatness': None, 'band_energy': None}
    bin_hz = 48000 / 4096
    centroid = sum(i*bin_hz*p for i, p in enumerate(power)) / total
    cumulative = 0
    rolloff = 24000
    for i, p in enumerate(power):
        cumulative += p
        if cumulative >= total * 0.85:
            rolloff = i * bin_hz
            break
    mean_power = total / len(power)
    flatness = math.exp(sum(math.log(p + 1e-15) for p in power) / len(power)) / mean_power
    bands = {}
    for lower, upper in ((20, 150), (150, 500), (500, 2000), (2000, 8000), (8000, 24000)):
        bands[f'{lower}-{upper}'] = round(sum(power[i] for i in range(len(power))
            if lower <= i*bin_hz < upper) / total, 6)
    return {'centroid_hz': round(centroid, 2), 'rolloff_85_hz': round(rolloff, 2),
            'flatness': round(flatness, 6), 'band_energy': bands}


def metrics(path):
    samples = audio(path)
    start = round(MANIFEST['pre_trigger_ms'] * 48)
    planned = dict(cases()).get(path.stem)
    if planned is None:
        raise ValueError(f'{path}: not in current case plan')
    expected = duration_seconds(planned) * 48000
    if abs(len(samples) - expected) > 2:
        raise ValueError(f'{path}: expected {expected} frames, got {len(samples)}')
    pre_peak = max((abs(x) for x in samples[:start]), default=0) / 32768
    if pre_peak > 0.001:
        raise ValueError(f'{path}: pre-trigger audio peak {pre_peak:.4f}; isolate Gearmulator before measuring')
    body = samples[start:]
    peak = max((abs(x) for x in body), default=0) / 32768
    windows = {}
    for ms in (0, 20, 50, 100, 200, 400, 800, 1200):
        at = ms * 48
        windows[str(ms)] = round(rms(body[at:at + 2400]), 7)
    pitch_windows = {}
    for ms in (20, 50, 100, 200):
        at = ms * 48
        pitch_windows[str(ms)] = pitch(body[at:at + 2400])
    initial = max(windows.values(), default=0)
    decay = None
    if initial > 0:
        threshold = initial * 0.01
        for ms in range(0, max(0, (len(body) - 2400) // 48), 10):
            at = ms * 48
            if rms(body[at:at + 2400]) <= threshold:
                decay = ms
                break
    spectral = spectral_metrics(body[960:960+4096])
    return {'file': path.name, 'peak': round(peak, 7), 'rms': windows,
            'pitch_hz': pitch_windows, 'decay_t40_ms': decay,
            'duration_ms': len(samples) / 48, 'tail_censored': decay is None,
            'spectrum_20ms': spectral}


def measure(folder):
    result = {}
    rejected = {}
    planned_names = {name for name, _ in cases()}
    for path in sorted(folder.glob('*.wav')):
        if path.stem not in planned_names:
            continue
        try:
            result[path.stem] = metrics(path)
        except ValueError as exc:
            rejected[path.stem] = str(exc)
    target = MACHINE / 'measurements' / (folder.name + '.json')
    target.write_text(json.dumps(result, indent=2) + '\n')
    if rejected:
        (MACHINE / 'measurements' / (folder.name + '_rejected.json')).write_text(
            json.dumps(rejected, indent=2) + '\n')
        print(f'Omitted {len(rejected)} invalid {folder.name} WAVs')
    else:
        (MACHINE / 'measurements' / (folder.name + '_rejected.json')).unlink(missing_ok=True)
    return result


def compare(ref, cand):
    output = {}
    for name in sorted(ref.keys() & cand.keys()):
        a, b = ref[name], cand[name]
        fields = {}
        for key in ('peak', 'decay_t40_ms'):
            if a[key] is not None and b[key] is not None:
                fields[key + '_error'] = round(b[key] - a[key], 5)
        for key in ('rms', 'pitch_hz'):
            fields[key + '_error'] = {t: round(b[key][t] - value, 5)
                 for t, value in a[key].items() if value is not None and b[key][t] is not None}
        fields['spectrum_20ms_error'] = {}
        for key in ('centroid_hz', 'rolloff_85_hz', 'flatness'):
            x, y = a['spectrum_20ms'][key], b['spectrum_20ms'][key]
            if x is not None and y is not None:
                fields['spectrum_20ms_error'][key] = round(y-x, 5)
        x, y = a['spectrum_20ms']['band_energy'], b['spectrum_20ms']['band_energy']
        if x is not None and y is not None:
            fields['spectrum_20ms_error']['band_energy'] = {
                band: round(y[band]-value, 6) for band, value in x.items()}
        output[name] = fields
    return output


def fit(reference):
    """Fit linear and exponential candidates; retain the lower normalized error."""
    fitted = {}
    for parameter in MANIFEST['parameters']:
        points = []
        for value in MANIFEST['sweep_values']:
            entry = reference.get(f'{parameter.lower()}_{value:03}')
            if entry and entry['decay_t40_ms'] is not None:
                points.append((value, entry['decay_t40_ms']))
        if len(points) < 4:
            continue
        def regression(rows):
            n = len(rows)
            sx = sum(x for x, _ in rows)
            sy = sum(y for _, y in rows)
            sxx = sum(x*x for x, _ in rows)
            sxy = sum(x*y for x, y in rows)
            slope = (n*sxy-sx*sy) / (n*sxx-sx*sx)
            return (sy-slope*sx)/n, slope
        offset, slope = regression(points)
        candidates = [('linear', offset, slope,
                       sum((offset+slope*x-y)**2 for x, y in points))]
        if all(y > 0 for _, y in points):
            a, b = regression([(x, math.log(y)) for x, y in points])
            candidates.append(('exponential', math.exp(a), b,
                               sum((math.exp(a+b*x)-y)**2 for x, y in points)))
        kind, a, b, error = min(candidates, key=lambda row: row[3])
        mean = sum(y for _, y in points) / len(points)
        total = sum((y-mean)**2 for _, y in points)
        fitted[parameter] = {'target': 'decay_t40_ms', 'mapping': kind,
            'a': round(a, 7), 'b': round(b, 7),
            'r_squared': round(1-error/total, 4) if total else None,
            'points': len(points), 'status': 'candidate fit; inspect before DSP use'}
    return fitted


def consistency_audit(reference):
    base = reference.get('baseline')
    if not base:
        return {'passed': False, 'reason': 'baseline missing', 'anchors': {}}
    anchors = {}
    for name in list(MANIFEST['parameters']) + ['_'.join(pair) for pair in MANIFEST['interactions']]:
        case = f'anchor_{name.lower()}'
        sample = reference.get(case)
        if not sample:
            anchors[case] = {'status': 'missing or rejected'}
            continue
        peak_ratio = sample['peak'] / base['peak'] if base['peak'] else None
        rms_ratio = sample['rms']['0'] / base['rms']['0'] if base['rms']['0'] else None
        anchors[case] = {'peak_ratio': round(peak_ratio, 4),
                         'early_rms_ratio': round(rms_ratio, 4),
                         'status': 'pass' if 0.9 <= peak_ratio <= 1.1 and
                          0.9 <= rms_ratio <= 1.1 else 'drift'}
    return {'passed': all(x['status'] == 'pass' for x in anchors.values()),
            'anchors': anchors}


def render_reference(selected_group=None):
    """Reopen the saved E1 preset for each group in a separate Reaper process."""
    project = ROOT / 'tests/md_reference/scripts/gearmulator.RPP'
    script = ROOT / 'tests/md_reference/scripts/render_reference.lua'
    reaper = pathlib.Path('/Applications/REAPER.app/Contents/MacOS/REAPER')
    if not project.exists() or not reaper.exists():
        raise SystemExit('Need saved gearmulator.RPP and Reaper CLI')
    selected = [(group, rows) for group, rows in groups()
                if not selected_group or group == selected_group]
    if not selected:
        raise SystemExit(f'Unknown group: {selected_group}')
    for group, rows in selected:
        with tempfile.TemporaryDirectory(prefix='md_reference_') as temporary:
            temp = pathlib.Path(temporary)
            case_file = temp / 'cases.tsv'
            done_file = temp / 'done.txt'
            case_file.write_text(''.join(f'{name}\t{",".join(map(str, patch))}\t{duration_seconds(patch)}\n'
                                         for name, patch in rows))
            env = os.environ.copy()
            env['DD_CASE_FILE'] = str(case_file)
            env['DD_DONE_FILE'] = str(done_file)
            process = subprocess.Popen([str(reaper), '-newinst', str(project), str(script)],
                                       env=env, start_new_session=True,
                                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            deadline = time.monotonic() + 90 + sum(duration_seconds(p) for _, p in rows) * 3
            try:
                while time.monotonic() < deadline:
                    if done_file.exists():
                        result = done_file.read_text().strip()
                        if result != 'ok':
                            raise RuntimeError(f'{group}: {result}')
                        print(f'Captured {group}: {len(rows)} cases', flush=True)
                        break
                    if process.poll() is not None:
                        raise RuntimeError(f'{group}: Reaper exited {process.returncode}')
                    time.sleep(0.5)
                else:
                    raise TimeoutError(f'{group}: Reaper render timed out')
            finally:
                if process.poll() is None:
                    os.killpg(process.pid, signal.SIGTERM)
                    try:
                        process.wait(timeout=10)
                    except subprocess.TimeoutExpired:
                        os.killpg(process.pid, signal.SIGKILL)
                        process.wait()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('action', choices=('render-reference', 'render-candidate', 'measure-reference', 'measure-candidate', 'compare', 'fit-reference', 'audit-reference', 'list-cases'))
    parser.add_argument('--renderer', default=str(ROOT / 'out/render_machine'))
    parser.add_argument('--group', help='Render only one sweep group')
    args = parser.parse_args()
    if args.action == 'list-cases':
        for name, patch in cases():
            print(name, ','.join(map(str, patch)))
    elif args.action == 'render-reference':
        render_reference(args.group)
    elif args.action == 'render-candidate':
        folder = MACHINE / 'candidate'
        folder.mkdir(exist_ok=True)
        selected = [pair for group, rows in groups() if not args.group or group == args.group
                    for pair in rows]
        if not selected:
            raise SystemExit(f'Unknown group: {args.group}')
        for name, patch in selected:
            subprocess.run([args.renderer, '--machine', 'trx_b2', '--params', ','.join(map(str, patch)),
                '--frames', str(duration_seconds(patch) * 48000), '--trigger-frame', '12000', '--output', str(folder / (name + '.wav'))], check=True)
    elif args.action.startswith('measure-'):
        data = measure(MACHINE / args.action.removeprefix('measure-'))
        print(f'Measured {len(data)} WAVs')
    elif args.action == 'fit-reference':
        ref = measure(MACHINE / 'reference')
        if not ref:
            raise SystemExit('No reference WAVs; capture Gearmulator first')
        audit = consistency_audit(ref)
        (MACHINE / 'report/consistency.json').write_text(json.dumps(audit, indent=2) + '\n')
        if not audit['passed']:
            (MACHINE / 'measurements/fitted.json').unlink(missing_ok=True)
            raise SystemExit('Default-equivalent reference renders drift; fitted curves withheld')
        target = MACHINE / 'measurements/fitted.json'
        target.write_text(json.dumps(fit(ref), indent=2) + '\n')
        print(target)
    elif args.action == 'audit-reference':
        ref = measure(MACHINE / 'reference')
        audit = consistency_audit(ref)
        target = MACHINE / 'report/consistency.json'
        target.write_text(json.dumps(audit, indent=2) + '\n')
        print(f'Consistency {"passed" if audit["passed"] else "failed"}: {target}')
    else:
        ref = measure(MACHINE / 'reference')
        cand = measure(MACHINE / 'candidate')
        if not ref:
            raise SystemExit('No reference WAVs; capture Gearmulator first')
        report = compare(ref, cand)
        target = MACHINE / 'report/comparison.json'
        target.write_text(json.dumps(report, indent=2) + '\n')
        print(f'Compared {len(report)} matching cases: {target}')


if __name__ == '__main__':
    main()
