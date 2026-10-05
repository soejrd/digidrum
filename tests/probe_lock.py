"""Exercise PULSE BD's former sample slot as a step parameter lock in Digiemu."""
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "digiemu"))
from emu import panel, portable, session  # noqa: E402

FW = ROOT / "digidrum/out/emulator/firmware/dt1-d003-69c2ab90"
portable.apply_env(portable.open_paths(str(FW)))
OUT = ROOT / "digidrum/out/lock-probe-d003"
OUT.mkdir(parents=True, exist_ok=True)
s = session.Session(str(FW / "snapshots/Digitakt_OSD003/gui.snap"),
                    str(FW / "Digitakt_OSD003.syx"), audio=True)

def capture(name):
    frame = s.screen_at(s.ms)
    if frame is not None:
        panel.write_png(frame, str(OUT / (name + ".png")))
    return {
        "ms": s.ms,
        "vp_d": int.from_bytes(s.m.uc.mem_read(0x8000279A, 2), "big"),
        "pcm_bytes": len(s.pcm),
    }

try:
    s.run_ms(300)
    s.press(s.code("FUNC"))
    s.tap(s.code("SRC"))
    s.release(s.code("FUNC"))
    for _ in range(4):
        s.tap(s.code("DOWN"))
    s.tap(s.code("YES"))
    s.tap(s.code("YES"))
    s.tap(s.code("SRC"))
    s.run_ms(300)
    states = {"machine": capture("machine")}
    s.tap(s.code("RECORD"))
    states["record"] = capture("record")
    s.press(s.code("1"))
    s.run_ms(300)
    states["trig_held"] = capture("trig_held")
    for _ in range(5):
        s.turn(s.encoder("D"), 1, after_ms=300)
    states["lock_held"] = capture("lock_held")
    s.release(s.code("1"))
    s.run_ms(500)
    states["trig_released"] = capture("trig_released")
    s.press(s.code("1"))
    s.run_ms(500)
    states["trig_reheld"] = capture("trig_reheld")
    s.release(s.code("1"))
    s.run_ms(300)
    result = {"halted": s.halted, "states": states, "inputs": s.inputs}
    (OUT / "report.json").write_text(json.dumps(result, indent=2))
    print(json.dumps(result, indent=2))
finally:
    s.close()
