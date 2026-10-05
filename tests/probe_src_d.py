"""Reproduce SRC D in Digiemu and count candidate stock UI paths.

Run with Digiemu's Python: .venv/bin/python ../digidrum/tests/probe_src_d.py
This reads the local D003 build and writes screenshots under digidrum/out/.
"""
import json
import os
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
EMU = ROOT / "digiemu"
sys.path.insert(0, str(EMU))

from emu import panel, portable, session

FW_DIR = ROOT / "digidrum/out/emulator/firmware/dt1-d003-69c2ab90"
paths = portable.open_paths(str(FW_DIR))
portable.apply_env(paths)
snap = FW_DIR / "snapshots/Digitakt_OSD003/gui.snap"
syx = FW_DIR / "Digitakt_OSD003.syx"
out = ROOT / "digidrum/out/src-d-probe-d003"
out.mkdir(parents=True, exist_ok=True)

candidates = {
    0x47BE1188: "digidrum_d_dispatch",
    0x4003B5C0: "stock_d_sample_branch",
    0x4003B5E0: "stock_d_ordinary_branch",
    0x400309B0: "stock_page_setter",
    0x40030A28: "ordinary_setter_path",
    0x40039DA8: "sample_lookup_a",
    0x40039E80: "sample_lookup_b",
    0x4003AFE4: "sample_lookup_c",
    0x4003B448: "sample_lookup_d",
    0x4003B49E: "sample_lookup_e",
    0x4003B6D4: "sample_lookup_f",
    0x4003A430: "src_waveform_setup",
    0x4003B386: "browser_call_1",
    0x4003B5D0: "browser_call_2",
    0x4003B652: "browser_call_3",
    0x4003B6CE: "browser_call_4",
    0x4003B722: "browser_tail_1",
    0x4003B750: "browser_tail_2",
}
hits = {name: 0 for name in candidates.values()}
s = session.Session(str(snap), str(syx), audio=True)
try:
    for addr, name in candidates.items():
        def callback(_uc, _addr, _size, _data, label=name):
            hits[label] += 1
        s.at(addr, callback)

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
    before = s.screen_at(s.ms)
    if before is not None:
        panel.write_png(before, str(out / "before.png"))
    hits_before = dict(hits)
    s.turn(s.encoder("D"), 5, after_ms=600)
    after = s.screen_at(s.ms)
    if after is not None:
        panel.write_png(after, str(out / "after.png"))
    report = {
        "halted": s.halted,
        "ms": s.ms,
        "inputs": s.inputs,
        "hits_before_D": hits_before,
        "hits_during_D": {k: hits[k] - hits_before[k] for k in hits},
        "screens_differ": before != after,
    }
    (out / "report.json").write_text(json.dumps(report, indent=2))
    print(json.dumps(report, indent=2))
finally:
    s.close()
