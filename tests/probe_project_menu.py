"""Map the project save/reload UI on an isolated Digiemu card copy."""
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "digiemu"))
from emu import panel, portable, session  # noqa: E402

FW = ROOT / "digidrum/out/project-reload-probe/dt1-d003-69c2ab90"
portable.apply_env(portable.open_paths(str(FW)))
OUT = FW.parent / "menu"
OUT.mkdir(parents=True, exist_ok=True)
s = session.Session(str(FW / "snapshots/Digitakt_OSD003/gui.snap"),
                    str(FW / "Digitakt_OSD003.syx"), audio=True)

def capture(label):
    frame = s.screen_at(s.ms)
    if frame is not None:
        panel.write_png(frame, str(OUT / (label + ".png")))

try:
    s.run_ms(300)
    capture("initial")
    s.tap(s.code("GLOBAL"))
    capture("global")
    s.tap(s.code("YES"))
    capture("project")
    s.tap(s.code("NO"))
    capture("back")
    s.tap(s.code("NO"))
    s.press(s.code("FUNC"))
    s.tap(s.code("GLOBAL"))
    s.release(s.code("FUNC"))
    s.run_ms(300)
    capture("func_global")
    result = {"halted": s.halted, "inputs": s.inputs}
    (OUT / "report.json").write_text(json.dumps(result, indent=2))
    print(json.dumps(result, indent=2))
finally:
    s.close()
