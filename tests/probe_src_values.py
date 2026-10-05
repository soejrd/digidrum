"""Check PULSE BD SRC parameter memory across edits and page changes in Digiemu."""
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "digiemu"))
from emu import portable, session  # noqa: E402
from unicorn.m68k_const import (UC_M68K_REG_D0, UC_M68K_REG_D2,
                                UC_M68K_REG_D3, UC_M68K_REG_D5,
                                UC_M68K_REG_D6, UC_M68K_REG_A0,
                                UC_M68K_REG_A2, UC_M68K_REG_A3)  # noqa: E402

FW = ROOT / "digidrum/out/emulator/firmware/dt1-d003-69c2ab90"
portable.apply_env(portable.open_paths(str(FW)))
SNAP = FW / "snapshots/Digitakt_OSD003/gui.snap"
SYX = FW / "Digitakt_OSD003.syx"
OUT = ROOT / "digidrum/out/src-values-d003.json"
VP_BASE = 0x80002794
VP_STRIDE = 106
PARAM_OFFSETS = (0, 2, 4, 6, 8, 10, 12, 14)


def values(s):
    return [[int.from_bytes(s.m.uc.mem_read(VP_BASE + t * VP_STRIDE + o, 2), "big")
             for o in PARAM_OFFSETS] for t in range(8)]


s = session.Session(str(SNAP), str(SYX), audio=True)
try:
    comparisons = []
    entries = []
    def entry(uc, _addr, _size, _data):
        entries.append({name: uc.reg_read(reg) for name, reg in
                        (("param", UC_M68K_REG_D2), ("delta_or_ref", UC_M68K_REG_D3),
                         ("d5", UC_M68K_REG_D5), ("a0", UC_M68K_REG_A0),
                         ("a2", UC_M68K_REG_A2), ("a3", UC_M68K_REG_A3))})
    s.at(0x4003B5E0, entry)
    def compare(uc, _addr, _size, _data):
        comparisons.append({name: uc.reg_read(reg) for name, reg in
                            (("old", UC_M68K_REG_D6), ("new", UC_M68K_REG_D0),
                             ("param", UC_M68K_REG_D2), ("changed_flag", UC_M68K_REG_D3))})
    s.at(0x4003B63E, compare)
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
    before = values(s)
    for _ in range(5):
        s.turn(s.encoder("C"), 1, after_ms=300)
    after_c = values(s)
    c_entries = list(entries)
    c_comparisons = list(comparisons)
    for _ in range(5):
        s.turn(s.encoder("D"), 1, after_ms=300)
    after_d = values(s)
    after_d_comparisons = list(comparisons)
    s.tap(s.code("FLTR"))
    s.tap(s.code("SRC"))
    after_page = values(s)
    result = {
        "halted": s.halted,
        "before": before,
        "after_C": after_c,
        "after_D": after_d,
        "after_page_roundtrip": after_page,
        "changed_cells": [[t, p, before[t][p], after_d[t][p]]
                          for t in range(8) for p in range(8)
                          if before[t][p] != after_d[t][p]],
        "D_value_comparisons": after_d_comparisons,
        "ordinary_entries": entries,
        "C_entries": c_entries,
        "C_comparisons": c_comparisons,
    }
    assert after_c[0][2] > before[0][2], "SRC C did not change"
    assert after_d[0][3] > after_c[0][3], "SRC D did not change"
    assert after_page[0][3] == after_d[0][3], "SRC D changed after page round trip"
    assert s.halted is None, s.halted
    OUT.write_text(json.dumps(result, indent=2))
    print(json.dumps(result, indent=2))
finally:
    s.close()
