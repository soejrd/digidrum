"""Exercise two PULSE BD tracks on the same step in Digiemu."""
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "digiemu"))
from emu import panel, portable, session  # noqa: E402
from unicorn.m68k_const import UC_M68K_REG_A7  # noqa: E402

FW = ROOT / "digidrum/out/project-reload-probe/dt1-d003-69c2ab90"
portable.apply_env(portable.open_paths(str(FW)))
OUT = FW.parent / "two-tracks"
OUT.mkdir(parents=True, exist_ok=True)
s = session.Session(str(FW / "snapshots/Digitakt_OSD003/gui.snap"),
                    str(FW / "Digitakt_OSD003.syx"), audio=True)
render_hits = [0, 0]
render_block = [0]
triggers = []

def capture(label):
    frame = s.screen_at(s.ms)
    if frame is not None:
        panel.write_png(frame, str(OUT / (label + ".png")))
    return {"ms": s.ms,
            "render_machine": list(s.m.uc.mem_read(0x800018BC, 2)),
            "pcm_bytes": len(s.pcm),
            "render_hits": list(render_hits),
            "triggers": list(triggers)}

def select_track(number):
    s.press(s.code("TRK"))
    s.tap(s.code(str(number)))
    s.release(s.code("TRK"))
    s.run_ms(200)

def select_pulse():
    s.press(s.code("FUNC"))
    s.tap(s.code("SRC"))
    s.release(s.code("FUNC"))
    for _ in range(4):
        s.tap(s.code("DOWN"))
    s.tap(s.code("YES"))
    s.tap(s.code("YES"))
    s.tap(s.code("SRC"))
    s.run_ms(300)

try:
    def start_block(_uc, _addr, _size, _data):
        render_block[0] += 1
    s.at(0x47BE15F4, start_block)
    def voice_call(uc, _addr, _size, _data):
        sp = uc.reg_read(UC_M68K_REG_A7)
        voice = int.from_bytes(uc.mem_read(sp + 4, 4), "big")
        trigger = int.from_bytes(uc.mem_read(sp + 12, 4), "big")
        for track in range(2):
            if voice == 0x47BE2874 + 36 * track:
                render_hits[track] += 1
                if trigger:
                    triggers.append([render_block[0], track])
    s.at(0x47BE187A, voice_call)
    s.run_ms(300)
    select_pulse()
    states = {"track1": capture("track1")}
    select_track(2)
    states["selected2"] = capture("selected2")
    select_pulse()
    states["track2"] = capture("track2")
    s.tap(s.code("RECORD"))
    s.tap(s.code("1"))
    states["step2"] = capture("step2")
    select_track(1)
    s.tap(s.code("1"))
    states["step1"] = capture("step1")
    s.tap(s.code("RECORD"))
    s.tap(s.code("PLAY"))
    s.run_ms(2200)
    states["playing"] = capture("playing")
    blocks = {}
    for block, track in triggers:
        blocks.setdefault(block, set()).add(track)
    simultaneous = sorted(block for block, tracks in blocks.items()
                          if tracks == {0, 1})
    result = {"halted": s.halted, "states": states,
              "simultaneous_blocks": simultaneous, "inputs": s.inputs}
    assert s.halted is None, s.halted
    assert states["playing"]["render_machine"] == [8, 8], result
    assert simultaneous, result
    (OUT / "report.json").write_text(json.dumps(result, indent=2))
    print(json.dumps(result, indent=2))
finally:
    s.close()
