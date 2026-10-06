# Machinedrum reference pipeline: TRX-B2

The first machine is `machines/trx_b2`. Its JSON manifest is the shared case list. The TRX-B2 branch in `machines/trx_md.c` is a new model inferred from the corrected Gearmulator sweep. The first measured regression pass is complete; listening validation is pending.

## Reference capture contract

The supplied `scripts/gearmulator.RPP` stores TRX-B2 with empty bank E pattern 1. The headless runner launches a fresh isolated Reaper process for each parameter group. Each process waits 20 seconds for Gearmulator initialization, primes the controls with a short throwaway render, renders a default-patch anchor, then renders the group's cases. Restarting the project reloads E1 before the next group. The source project is never saved by the runner.

The shared plan contains one baseline, six values per control (`0, 25, 51, 76, 102, 127`), and only two 3×3 interactions: RAMP×HOLD and DEC×HOLD. Ten default-patch anchors check that groups match the baseline. All captures are mono 48 kHz with a MIDI note 36 at 250 ms. Render length is 2–44 seconds, increasing for high DEC and HOLD. It is a conservative allocation, and `tail_censored` in measurements flags a hit that has not reached −40 dB before the file ends. Generate the case list with:

```sh
python3 tests/md_reference/scripts/pipeline.py list-cases
```

Run the unattended reference suite with:

```sh
python3 tests/md_reference/scripts/pipeline.py render-reference
```

A single group can be run with `--group dec`. The generated WAVs go under `machines/trx_b2/reference`. Existing WAVs from the earlier 198-case trial are retained but excluded from current measurements. Invalid captures are omitted and recorded in `measurements/reference_rejected.json`; the runner does not retry or repair them automatically.

For state tests, also capture single hits and repeated triggers at 500, 100, and 20 ms and a 16th-note stream; record the exact tempo and trigger times alongside the WAVs. The current single-trigger measurement script does not analyze those recordings.

## Candidate and comparison

```sh
make out/render_machine
python3 tests/md_reference/scripts/pipeline.py render-candidate
python3 tests/md_reference/scripts/pipeline.py measure-reference
python3 tests/md_reference/scripts/pipeline.py audit-reference
python3 tests/md_reference/scripts/pipeline.py fit-reference
python3 tests/md_reference/scripts/pipeline.py measure-candidate
python3 tests/md_reference/scripts/pipeline.py compare
```

The direct renderer also accepts `--machine trx_b2 --params 64,64,64,0,64,0,0,0 --frames 96000 --trigger-frame 12000 --output out/example.wav`. It uses the same `machines/trx_md.c` source linked into firmware. The pipeline writes compact JSON measurements and comparison errors, including a 4096-point spectral summary. Pitch is estimated from positive zero crossings, so noisy or clipped patches need manual inspection. Reference WAVs with pre-trigger audio are omitted and named in `measurements/reference_rejected.json`. Fitting stops when equivalent default-patch renders drift; the primed 77-case run passed this audit. The current features do not yet cover detailed distortion profiling or state behavior.

The reference capture, first synthesis model, and first 77-case regression pass are complete. Listening, Digiemu, hardware, and CPU profiling remain before calling TRX-B2 validated. See `machines/trx_b2/REGRESSION.md`.

## Reaper automation

`tests/md_reference/scripts/gearmulator.RPP` is the user's source project and is ignored by Git because it embeds plugin state. `pipeline.py render-reference` feeds per-group case lists to `render_reference.lua`. The script invokes Reaper's auto-close render action and overwrites generated case WAVs. The matching `Gearmulator MD 2.vst3` must be installed for the headless Reaper process. Reaper's direct `-renderproject` option previously rendered silence because Gearmulator had not completed initialization.

The earlier coarse run produced 198 WAVs; 22 were omitted for pre-trigger carryover and default-equivalent cases drifted. Those old files are retained locally as evidence and are not part of the reduced plan. A first 77-case fresh-process run revealed that matching stored Reaper parameter values did not force Gearmulator internal values to update. The current runner primes controls before every group. Its 77 cases measured cleanly and all ten anchors passed. The old C TRX-B2 branch was replaced from these corrected measurements and is undergoing regression against them.
