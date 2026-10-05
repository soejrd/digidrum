"""Regression: stock SRC D retains its sample browser in the D003 firmware."""
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "digiemu"))
from emu import panel, portable, session  # noqa: E402

FW = ROOT / "digidrum/out/emulator/firmware/dt1-d003-69c2ab90"
portable.apply_env(portable.open_paths(str(FW)))
OUT = ROOT / "digidrum/out/stock-d-probe-d003"
OUT.mkdir(parents=True, exist_ok=True)
s = session.Session(str(FW / "snapshots/Digitakt_OSD003/gui.snap"),
                    str(FW / "Digitakt_OSD003.syx"), audio=True)
hits = {"dispatch": 0, "sample_branch": 0, "ordinary_branch": 0,
        "browser_call": 0}
try:
    for addr, name in ((0x47BE1188, "dispatch"), (0x4003B5C0, "sample_branch"),
                       (0x4003B5E0, "ordinary_branch"),
                       (0x4003B5D0, "browser_call")):
        def callback(_uc, _addr, _size, _data, key=name):
            hits[key] += 1
        s.at(addr, callback)
    s.run_ms(300)
    s.tap(s.code("SRC"))
    s.turn(s.encoder("D"), 1, after_ms=600)
    frame = s.screen_at(s.ms)
    if frame is not None:
        panel.write_png(frame, str(OUT / "after.png"))
    result = {"halted": s.halted, "hits": hits}
    assert s.halted is None, s.halted
    assert hits["sample_branch"] and hits["browser_call"], hits
    assert hits["ordinary_branch"] == 0, hits
    (OUT / "report.json").write_text(json.dumps(result, indent=2))
    print(json.dumps(result, indent=2))
finally:
    s.close()
