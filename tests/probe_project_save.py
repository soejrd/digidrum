"""Explore saving a PULSE BD project on an isolated Digiemu card clone."""
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "digiemu"))
from emu import panel, portable, session  # noqa: E402

FW = ROOT / "digidrum/out/project-reload-probe/dt1-d003-69c2ab90"
portable.apply_env(portable.open_paths(str(FW)))
OUT = FW.parent / "save"
OUT.mkdir(parents=True, exist_ok=True)
s = session.Session(str(FW / "snapshots/Digitakt_OSD003/gui.snap"),
                    str(FW / "Digitakt_OSD003.syx"), audio=True)

def capture(name):
    frame = s.screen_at(s.ms)
    if frame is not None:
        panel.write_png(frame, str(OUT / (name + ".png")))
    return {
        "ms": s.ms,
        "machine": s.m.uc.mem_read(0x800018BC, 1)[0],
        "d_value": int.from_bytes(s.m.uc.mem_read(0x8000279A, 2), "big"),
        "card_overlay_bytes": len(s.ev["esdhc"].card.overlay),
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
    states = {"selected": capture("selected")}
    for _ in range(5):
        s.turn(s.encoder("D"), 1, after_ms=300)
    states["edited"] = capture("edited")
    s.tap(s.code("GLOBAL"))
    states["settings"] = capture("settings")
    s.tap(s.code("YES"))
    states["project_menu"] = capture("project_menu")
    s.tap(s.code("YES"))
    states["project_action"] = capture("project_action")
    s.tap(s.code("YES"))
    states["create_new"] = capture("create_new")
    s.tap(s.code("YES"))
    states["confirmed"] = capture("confirmed")
    s.run_ms(1000)
    states["after_wait"] = capture("after_wait")
    result = {"halted": s.halted, "states": states, "inputs": s.inputs}
    (OUT / "report.json").write_text(json.dumps(result, indent=2))
    print(json.dumps(result, indent=2))
finally:
    s.close()
