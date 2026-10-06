# Machinedrum reference workspace

This directory contains the headless Gearmulator capture and host regression for TRX-B2. Start in the `digidrum` repository root. The reusable method and stopping rules are in [`Engineering/MACHINEDRUM_RECREATION_PIPELINE.md`](../../Engineering/MACHINEDRUM_RECREATION_PIPELINE.md).

## One-time setup

`scripts/gearmulator.RPP` is the user-saved Reaper project with Gearmulator MD, TRX-B2, and empty bank E pattern 1. It is ignored by Git because it contains plug-in state. The **exact** `Gearmulator MD 2.vst3` used by that project must be available to the headless Reaper process. Reaper is expected at `/Applications/REAPER.app/Contents/MacOS/REAPER`. Ask the user for a simple Reaper action if the project or plug-in needs correction; the routine capture needs no screen control.

The runner waits 20 seconds **once per parameter group** for Gearmulator to load. It then renders a deliberately different throwaway patch to synchronize Gearmulator's internal controls, records a base-patch anchor, and records that group's cases. A new Reaper process reloads the saved E1 patch before the next group. The source project is never saved by the runner. The Lua script deletes an existing case WAV before rendering it, so rerunning a group overwrites its generated files.

## Capture only what is needed

[`machines/trx_b2/machine.json`](machines/trx_b2/machine.json) is the case plan: one baseline; six values (`0, 25, 51, 76, 102, 127`) for each of eight controls; a base anchor in each group; and two 3×3 interactions, RAMP×HOLD and DEC×HOLD. That is 77 files. Every file has a 250 ms pre-trigger interval, MIDI note 36, mono 48 kHz audio, and a render duration that grows with both DEC and HOLD. Other combinations are omitted until evidence requires them.

```sh
python3 tests/md_reference/scripts/pipeline.py list-cases
python3 tests/md_reference/scripts/pipeline.py render-reference --group nois
python3 tests/md_reference/scripts/pipeline.py render-reference
```

Run a **single group** for a focused question; run the full suite for first characterization or after a shared reference-setup change. Reference WAVs go to `machines/trx_b2/reference/`. The earlier invalid 198-case run and unprimed 77-case run are historical evidence, not current fitting input. Extra diagnostic WAVs may sit beside the planned files; the measurement script ignores names outside the manifest.

## Measure and audit before fitting

```sh
python3 tests/md_reference/scripts/pipeline.py measure-reference
python3 tests/md_reference/scripts/pipeline.py audit-reference
python3 tests/md_reference/scripts/pipeline.py fit-reference
```

`measure-reference` rejects files with pre-trigger audio or unexpected length and records reasons in `measurements/reference_rejected.json`. A missing or drifting anchor invalidates the group: fix the capture state before interpreting a curve. Check `tail_censored` before using a decay measurement; extend that case if needed. The corrected 77-case reference passed all ten anchor checks with no rejected or censored files.

`fit-reference` first runs the anchor audit. It currently fits **only −40 dB time** with linear/exponential candidates, even for controls whose real target is pitch or tone. Treat `measurements/fitted.json` as a prompt for inspection. [`machines/trx_b2/CHARACTERIZATION.md`](machines/trx_b2/CHARACTERIZATION.md) records the first model; [`machines/trx_b2/REGRESSION.md`](machines/trx_b2/REGRESSION.md) records the later NOIS and tail corrections.

## Render the candidate and compare

```sh
make out/render_machine
python3 tests/md_reference/scripts/pipeline.py render-candidate --group nois
python3 tests/md_reference/scripts/pipeline.py measure-candidate
python3 tests/md_reference/scripts/pipeline.py compare
```

The direct renderer compiles the same `machines/trx_md.c` used by firmware and writes 24-bit PCM so quiet tails remain inspectable. For a specific patch, render one file without the full pipeline:

```sh
out/render_machine --machine trx_b2 --params 64,127,64,0,64,127,0,0 --frames 96000 --trigger-frame 12000 --output out/nois-check.wav
```

The eight comma-separated controls are `PTCH,DEC,RAMP,HOLD,TICK,NOIS,DIRT,DIST`. `--frames` includes the 12,000-frame pre-trigger interval. Choose a longer value when DEC or HOLD is high; the two-second command above is only an attack check. Full case comparisons are written to `report/comparison.json`; [`machines/trx_b2/REGRESSION.md`](machines/trx_b2/REGRESSION.md) carries the compact conclusions. Do not print the entire JSON report to inspect one parameter. `compare` reads all valid files, so a group-only render can leave older candidate files for other groups. Rerender all groups before quoting a new full-suite result.

## Efficient regression loop

1. State one audible or measured mismatch and choose a few cases that expose it.
2. Reuse valid reference WAVs. Capture new Gearmulator audio only for a missing time window, threshold, or interaction.
3. Measure the relevant window with absolute as well as normalized levels. NOIS needed 0–70 ms high-frequency bands; the metallic tail needed clean-body spectra at 200–500 ms. A 20 ms centroid alone missed both problems.
4. Change the C model and rerender only the affected group. Run all 77 cases after shared DSP changes or before handing off a firmware image.
5. Run `make test cross-check`, mod lint, and SysEx patch verification for a target build. Ask for a short, specific listening check; use digihealth on the Digitakt Mk1 for CPU/DSP measurements.

The current measurement code estimates pitch from positive zero crossings, which can misread noise or clipped sounds. It also does not characterize retrigger state or detailed distortion texture. Add targeted tests for those questions instead of expanding every sweep by default.
