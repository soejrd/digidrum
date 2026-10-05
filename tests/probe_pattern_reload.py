"""Check Digidrum sound parameter through pattern save/reload shortcuts."""
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "digiemu"))
from emu import panel, portable, session  # noqa: E402

FW = ROOT / "digidrum/out/project-reload-probe/dt1-d003-69c2ab90"
portable.apply_env(portable.open_paths(str(FW)))
OUT = FW.parent / "pattern-reload"
OUT.mkdir(parents=True, exist_ok=True)
s = session.Session(str(FW / "snapshots/Digitakt_OSD003/gui.snap"),
                    str(FW / "Digitakt_OSD003.syx"), audio=True)

def capture(label):
    frame = s.screen_at(s.ms)
    if frame is not None:
        panel.write_png(frame, str(OUT / (label + ".png")))
    return {"ms": s.ms,
            "C": int.from_bytes(s.m.uc.mem_read(0x80002798, 2), "big"),
            "D": int.from_bytes(s.m.uc.mem_read(0x8000279A, 2), "big")}

def func_tap(name):
    s.press(s.code("FUNC"))
    s.tap(s.code(name))
    s.release(s.code("FUNC"))
    s.run_ms(300)

try:
    s.run_ms(300)
    func_tap("SRC")
    for _ in range(4):
        s.tap(s.code("DOWN"))
    s.tap(s.code("YES"))
    s.tap(s.code("YES"))
    s.tap(s.code("SRC"))
    s.run_ms(300)
    states = {"base": capture("base")}
    func_tap("YES")
    states["saved"] = capture("saved")
    for _ in range(5):
        s.turn(s.encoder("D"), 1, after_ms=300)
    for _ in range(5):
        s.turn(s.encoder("C"), 1, after_ms=300)
    states["edited"] = capture("edited")
    func_tap("NO")
    states["reload"] = capture("reload")
    s.tap(s.code("1"))
    states["retrigger"] = capture("retrigger")
    s.tap(s.code("FLTR"))
    s.tap(s.code("SRC"))
    states["page_roundtrip"] = capture("page_roundtrip")
    result = {"halted": s.halted, "states": states, "inputs": s.inputs}
    (OUT / "report.json").write_text(json.dumps(result, indent=2))
    print(json.dumps(result, indent=2))
finally:
    s.close()
