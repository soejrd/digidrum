# Machinedrum recreation pipeline

Recreate one Machinedrum machine at a time as independent Digidrum C DSP. Gearmulator is the black-box reference; Reaper captures it headlessly. The target is convincing sound and parameter behavior, including quiet tails and transitions between knob values. Sample-by-sample waveform identity is not the goal.

The working TRX-B2 implementation and commands are documented in [`tests/md_reference/README.md`](../tests/md_reference/README.md). This document describes the process to reuse for another machine.

## Operating budget

| Stage | Default work | Expand only when |
| --- | --- | --- |
| Setup | One user-saved Reaper project with an empty pattern and known base patch | The plug-in or machine is unavailable headlessly |
| Reference | Baseline, six values per control, one baseline anchor per group | A capture fails, a curve has a knee, or a specific interaction is suspected |
| Interactions | At most two justified 3×3 grids | Evidence shows another pair changes behavior |
| Analysis | Compact feature JSON and a short characterization | Features and listening disagree |
| Regression | Host renders for affected groups, then one full comparison at a milestone | A change touches shared DSP or state |
| Listening | A few named patches and the failure being investigated | The user finds another audible mismatch |

Keep the WAVs as evidence on disk. Give an agent the manifest, compact measurements, characterization, and a few relevant report rows. Do not paste whole WAVs or full 77-case JSON reports into a conversation.

## 1. Establish a trustworthy reference

Ask the user for simple one-time Reaper actions when needed, such as selecting an empty pattern, saving a base preset, or confirming a plug-in installation. Avoid screen control and repeated visual inspection. Verify the exact plug-in variant that the project references; a similarly named VST3 may render silence.

The TRX-B2 run showed four failure modes worth checking before any fitting:

1. Gearmulator needs about **20 seconds after Reaper opens** before rendering. Reaper's direct `-renderproject` path produced silent files during initialization.
2. Reaper can report the requested parameter value while Gearmulator internally retains a different kit value. Apply a deliberately different throwaway patch, render it, then set the base values before recording measurements.
3. Internal state can leak between captures. Reopen the saved project for each parameter group to restore the base pattern and patch; render a baseline anchor before the group's sweep. Do not pay the 20-second boot cost for every case.
4. A nonzero pre-trigger region means the file is not an isolated hit. Reject it rather than fitting around it.

The current headless runner starts one isolated Reaper process per group, waits once, primes once, renders its anchor and cases, then closes. It overwrites generated WAVs when a group is rerun. The saved source project is not modified. If a group fails its anchor, fix reference state before changing C code.

## 2. Design the smallest useful capture plan

Store the machine ID, eight control names, base patch, sweep points, sample rate, trigger time, render length rule, and selected interactions in a manifest. Use one control at a time with **six values across 0–127**; TRX-B2 uses `0, 25, 51, 76, 102, 127` plus its base value of 64. Capture the baseline once and an equivalent baseline at the start of each group.

TRX-B2 used 57 cases for the baseline, eight six-point sweeps, and eight anchors. Two justified 3×3 interactions plus their anchors brought the final plan to 77 cases. Do not calculate every pairwise combination. Add a few points around a threshold only after a coarse sweep or listening identifies it.

Use a fixed trigger at 250 ms, mono 48 kHz lossless WAV, fixed velocity, and no effects or changing LFO state. Allocate the render by **DEC and HOLD together**; high values can last tens of seconds. A `tail_censored` result means the hit did not fall below the chosen threshold within the file and must not be treated as a measured decay time. Extend that case, not the whole suite.

For noise or random machines, capture a small set of repeated hits to estimate variation. For stateful behavior, add focused retrigger intervals only after the single-hit model works. Keep these tests separate from the basic sweep.

## 3. Measure, audit, then fit

Extract a small feature set that describes the machine: peak, RMS at several post-trigger times, a −40 dB time, reliable pitch observations, and a few absolute and relative spectral bands. Choose attack and tail windows appropriate to the sound. A single 20 ms centroid can miss a wrong noise duration, distortion texture, or a metallic tail.

Before fitting, require:

- no pre-trigger audio;
- expected frame count and audio format;
- no unexplained baseline-anchor drift;
- no tail censoring for values used as decay observations.

Fit the **measured behavior**, not an assumed knob curve. For example, fit DEC to amplitude time constants and PTCH to late fundamental frequency. Start with linear, exponential, or a small piecewise table. A six-point table is often cheaper and more honest than a poorly justified equation. Check the fit against a held-out or targeted value near each suspected knee. Report the quantity fitted and residual error; an R² value alone is not validation.

The current `fit-reference` command is exploratory: it compares linear and exponential fits of `decay_t40_ms` for every control. Its output is a candidate diagnostic, **not** eight ready-to-use DSP mappings. Noise and clipped patches can also fool the simple zero-crossing pitch estimator; inspect a short waveform or spectrum only for those cases.

Write one short `CHARACTERIZATION.md` containing measured facts, the simplest synthesis hypothesis, uncertainties, and any important interactions. Distinguish a measurement from an inference about the hidden engine.

## 4. Implement and compare cheaply

Compile the **same C source** for the host renderer and ColdFire firmware. Build the simplest topology consistent with the measurements. Keep costly mapping work outside the audio loop and avoid unsupported 64-bit target arithmetic. Preserve enough internal precision for long decays; truncating a quiet body to 16 bits or stepping envelopes in large blocks can sound metallic even if broad envelope metrics match.

Use the same cases and measurement code for reference and candidate. On each edit:

1. Render and inspect the affected group or a few direct cases.
2. Compare the feature that motivated the edit in the relevant time window. For NOIS, inspect high-frequency energy at the attack and 10–70 ms, not only global RMS. For an alias-like tail, inspect the clean body at 200–500 ms. For DIRT, compare values immediately on both sides of a threshold.
3. Run the full suite after shared topology, envelope, or output-precision changes, or before an emulator/hardware handoff.
4. Ask for a short listening check of the specific unresolved sound. Fix that mismatch before broadening the capture plan.

Compare absolute band levels as well as normalized spectra: a candidate can have a similar centroid while its noise burst is much too quiet. Broad features do not prove sonic identity. Avoid fitting phase-aligned samples when two valid oscillators can differ by phase.

TRX-B2 illustrates a productive iteration: the first 77-case pass matched DEC and HOLD well but listening found short, dull NOIS and a metallic tail. Three long NOIS references identified a bright 50–70 ms burst. A clean-tail comparison then exposed held half-rate output, quantization bias, and stepped envelopes. Targeted fixes and one full regression were more useful than a denser sweep of every parameter combination.

## 5. Validate the target

Before a user test, run host tests, ColdFire compilation/linking, mod lint, and SysEx patch verification. The host renderer should retain at least 24-bit PCM when inspecting low-level tail artifacts. Digiemu listening checks sound and controls; it does not establish physical CPU headroom. On the Digitakt Mk1, measure one voice, repeated triggers, and worst-case simultaneous voices with digihealth, first with FAST AUDIO off. Record DSP current/peak and CPU load, then repeat with FAST AUDIO on if needed. A well-formed `.syx` is not proof that hardware timing is safe.

## Stop rules and handoff

Stop collecting when the anchors are stable, each important control is characterized at the coarse points, relevant tails are uncensored, and remaining questions can be named precisely. Stop tuning when measured envelope/pitch and targeted spectra are acceptably close, the user confirms the sound, and hardware behavior fits the CPU budget. If a mismatch remains, record it explicitly rather than hiding it behind a single aggregate score.

For the next worker, provide the manifest, characterization, compact regression table, exact commands, and **one next uncertainty**. Point to raw WAV paths only when that uncertainty needs waveform inspection. This keeps the process reproducible without spending tokens rediscovering the whole run.
