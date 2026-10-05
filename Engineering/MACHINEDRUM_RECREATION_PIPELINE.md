# Machinedrum Recreation Pipeline

## Purpose

This document defines the repeatable process for recreating Machinedrum instruments as new C DSP code for Digidrum.

The goal is not firmware emulation or waveform identity. The goal is a close clean-room approximation of each machine's audible behavior and parameter response.

Reference system:

- Gearmulator running user-owned Machinedrum firmware
- Reaper for deterministic automation and offline rendering
- automated measurement and curve fitting
- Digidrum C code as the candidate implementation

The basic loop is:

```text
Gearmulator reference
        ↓
automated parameter sweeps
        ↓
compact measurements + fitted behavior
        ↓
synthesis hypothesis
        ↓
C implementation
        ↓
candidate renders
        ↓
automated comparison
        ↓
iterate
        ↓
hardware validation
```

---

# 1. Core rule

Treat the Machinedrum as a black-box reference.

For each machine:

1. render controlled reference hits
2. measure the important behavior
3. infer the simplest useful synthesis model
4. implement it independently in C
5. render the candidate with the same test cases
6. compare behavior
7. refine mappings and constants
8. validate on Digitakt hardware

Do not optimize for raw sample-by-sample similarity. Small phase differences make that misleading.

Focus on:

- pitch and pitch envelope
- amplitude envelope
- transient shape
- harmonic/noise balance
- spectral shape
- distortion/quantization behavior
- parameter curves
- important parameter interactions
- retrigger/state behavior

---

# 2. Tools

## Gearmulator

Use Gearmulator as the reference Machinedrum.

Reference tests should use:

- one target machine
- fixed initial state
- no LFO or effects
- neutral track processing where possible
- fixed velocity/accent
- fixed trigger timing

## Reaper

Use one permanent Reaper project for reference rendering.

Reaper is responsible for:

- hosting Gearmulator
- selecting the machine
- setting parameters
- triggering hits
- offline rendering
- batch sweeps

Prefer a ReaScript so the full process is repeatable.

## Candidate renderer

The same C source used by Digidrum should also compile into a small host-side renderer.

Preferred structure:

```text
instrument C source
    ├── ColdFire firmware build
    └── host test renderer
```

Avoid maintaining a separate desktop version of the DSP.

---

# 3. Repository layout

Recommended:

```text
tests/
  md_reference/
    machines/
      trx_b2/
        machine.yaml
        reference/
        measurements/
        candidate/
        report/
      trx_bd/
      trx_sd/
      ...
    scripts/
      render_reference.py
      render_candidate.py
      analyze_audio.py
      compare_machine.py

tools/
  reaper/
    md_reference_project.rpp
    scripts/
      render_machine.lua
      render_sweep.lua

instruments/
  trx_b2.c
  trx_b2.h
  ...
```

Use one directory per Machinedrum machine.

---

# 4. Machine manifest

Each machine gets a manifest describing its reference controls and Digidrum mapping.

Example:

```yaml
id: trx_b2
reference_name: TRX-B2
family: TRX

reference_parameters:
  - PTCH
  - DEC
  - RAMP
  - HOLD
  - TICK
  - NOIS
  - DIRT
  - DIST

digidrum_parameters:
  - TUNE
  - DECAY
  - SWEEP
  - HOLD
  - ATTACK
  - DIRT
  - DIST

default_reference_patch:
  PTCH: 64
  DEC: 64
  RAMP: 64
  HOLD: 0
  TICK: 64
  NOIS: 0
  DIRT: 0
  DIST: 0
```

Document deliberate parameter merges or differences here.

---

# 5. Reference rendering

Use a fixed render window, for example:

```text
250 ms silence
trigger
1500 ms response
250 ms silence
```

Longer machines can use a larger window.

Store reference renders as lossless WAV.

WAV files are the ground truth, but they are **not** the main format given to AI agents.

---

# 6. AI-facing audio data

Raw audio becomes expensive and unwieldy when many parameters are swept.

Use four data levels:

```text
LEVEL 0 — WAV
raw reference evidence

LEVEL 1 — measurements
compact JSON/CSV extracted from WAVs

LEVEL 2 — fitted behavior
curve equations, interaction models, classifications

LEVEL 3 — characterization
short Markdown summary used by developers and agents
```

Most agents should work from Levels 2 and 3.

Raw WAVs remain available when a result needs to be checked again.

## Example measurement output

Instead of giving an agent many WAV files:

```json
{
  "parameter": "RAMP",
  "points": [
    {
      "value": 0,
      "f0_start_hz": 55.2,
      "f0_100ms_hz": 54.6,
      "decay_t40_ms": 285,
      "spectral_centroid_hz": 310
    },
    {
      "value": 64,
      "f0_start_hz": 132.4,
      "f0_100ms_hz": 56.0,
      "decay_t40_ms": 282,
      "spectral_centroid_hz": 490
    }
  ]
}
```

Then reduce that further into a fitted model:

```json
{
  "parameter": "RAMP",
  "target": "pitch_env_depth",
  "mapping": "power",
  "expression": "2.73 * x^1.91",
  "confidence": 0.96,
  "interactions": ["PTCH", "HOLD"]
}
```

The fitted model is usually the most useful agent input.

---

# 7. What to measure

## Tonal / kick / tom machines

Measure:

- fundamental frequency
- pitch trajectory
- pitch sweep depth
- pitch sweep time
- attack time
- decay time
- amplitude envelope
- transient energy
- harmonic peaks
- spectral centroid
- distortion profile

## Noise / hat / cymbal machines

Measure:

- amplitude envelope
- spectral centroid
- spectral rolloff
- spectral flatness
- energy by frequency band
- dominant resonant peaks
- transient duration
- variation between repeated hits

## Stateful or stochastic machines

Render repeated hits and summarize statistics instead of relying on one WAV.

Example:

```json
{
  "hits": 32,
  "peak_db_mean": -3.8,
  "peak_db_std": 0.6,
  "centroid_mean_hz": 8240,
  "centroid_std_hz": 510,
  "hit_correlation_mean": 0.21
}
```

---

# 8. Parameter sweeps

Start with a coarse sweep:

```text
0
16
32
48
64
80
96
112
127
```

Keep all other parameters fixed.

If the result is smooth and simple, fit the curve and stop.

If the result shows:

- discontinuities
- mode changes
- strong curvature
- unexpected interaction
- unstable behavior

then rerun that area at higher resolution.

Do not render dense sweeps by default.

---

# 9. Parameter classification

Classify each control after the first sweep:

```text
SIMPLE
smooth single mapping

COUPLED
depends strongly on another parameter

PIECEWISE
changes behavior at thresholds

STOCHASTIC
needs repeated-hit statistics

STATEFUL
depends on earlier triggers or internal state
```

This determines what testing is needed next.

---

# 10. Interaction tests

Only test parameter pairs that appear to interact.

Recommended grid:

```text
0
32
64
96
127
```

Example TRX-B2 pairs:

```text
PTCH x RAMP
RAMP x HOLD
TICK x NOIS
DIRT x DIST
DEC x HOLD
```

Summarize the result as a compact interaction model instead of exposing the whole matrix to agents.

Example:

```text
RAMP x HOLD

Interaction: moderate

HOLD extends the period during which the pitch sweep remains near
its starting value.

Approximation:
sweep_time = base_sweep * (1 + 1.8 * hold_norm)
```

---

# 11. Retrigger and state tests

Test:

```text
single hit
2 hits at 500 ms
2 hits at 100 ms
2 hits at 20 ms
16th-note stream
rapid retrigger
```

Determine whether:

- oscillator phase resets
- envelopes restart
- resonator state survives
- RNG state continues
- rapid hits accumulate energy
- distortion changes under overlap

Record the result in the machine characterization.

---

# 12. Parameter-curve fitting

For every control, identify the simplest useful mapping:

```text
linear
quadratic
power
exponential
logarithmic
piecewise
quantized
mode-switching
```

Example:

```text
DEC

Observed:
0   -> 18 ms
32  -> 42 ms
64  -> 123 ms
96  -> 405 ms
127 -> 1.74 s

Fit:
T60(x) = 0.0178 * exp(0.0361 * x)

R² = 0.995
```

Store fitted mappings separately from the audio loop.

Example:

```c
uint16_t trx_b2_map_decay(uint8_t x);
```

---

# 13. Machine characterization

Before implementing the C version, create a short `CHARACTERIZATION.md`.

Example:

```text
TRX-B2

Source:
sine/resonant body

PTCH:
controls base frequency
approximately exponential

RAMP:
controls downward pitch-envelope depth

HOLD:
holds amplitude before exponential decay
also interacts moderately with pitch sweep

TICK / NOIS:
short transient layer

DIRT:
digital quantization / degradation

DIST:
post-mix nonlinear stage

State:
oscillator phase appears to reset on trigger
```

Mark uncertain conclusions:

```text
MEASURED
STRONG INFERENCE
WEAK INFERENCE
UNKNOWN
```

Keep this file short.

---

# 14. C implementation

Implement the simplest topology that matches the measurements.

Use existing Digidrum helpers where appropriate:

- phase accumulator
- sine LUT
- recursive envelope
- ramp generator
- noise source
- resonator
- saturator
- quantizer
- half-rate renderer
- quarter-rate renderer

Rules:

1. no unnecessary floating point
2. no 64-bit target math
3. keep inner loops small
4. keep parameter mapping outside expensive inner loops
5. reuse helpers only when the behavior genuinely matches

---

# 15. Candidate rendering

The host renderer should support direct machine rendering.

Example:

```text
./render_machine \
    --machine trx_b2 \
    --params 64,64,64,0,64,0,0 \
    --frames 96000 \
    --output candidate.wav
```

Candidate renders go through the same measurement code as the reference.

---

# 16. Comparison

Compare features, not raw samples.

Useful comparison fields:

```text
pitch_error
pitch_curve_error
envelope_error
transient_error
spectral_error
distortion_error
state_behavior
```

Use different weighting for different machine types.

Kick example:

```text
pitch curve      high priority
amplitude env    high priority
transient        medium/high
spectral shape   medium
distortion       lower unless central to the patch
```

Hat/cymbal example:

```text
spectral shape   high priority
transient        high priority
decay            medium/high
pitch            low unless clearly tonal
```

Listening remains the final sanity check, but the pipeline should not depend on manually listening to every sweep.

---

# 17. Iteration

Use this order when improving a machine:

1. fix parameter mappings
2. fix envelope behavior
3. fix pitch behavior
4. fix transient structure
5. fix spectral balance
6. fix nonlinear character
7. only change topology if the current model cannot match an important measured behavior

Do not rewrite the algorithm every time a constant is wrong.

---

# 18. Target validation

Once the host model is good enough:

1. cross-compile for ColdFire
2. run unit tests
3. run overflow checks
4. test in Digiemu
5. test on real hardware
6. measure one-voice cost
7. measure worst-case simultaneous voices
8. test rapid retriggering
9. test extreme parameter combinations
10. verify stock filter, overdrive, mixer and sends still behave correctly

Record the CPU budget for every machine.

---

# 19. Per-machine completion checklist

```text
[ ] manifest created
[ ] baseline reference render complete
[ ] all parameters coarse-swept
[ ] complex parameters rerun at higher resolution where needed
[ ] important interactions tested
[ ] retrigger/state behavior tested
[ ] measurements generated
[ ] fitted behavior generated
[ ] CHARACTERIZATION.md written
[ ] C implementation complete
[ ] host renderer works
[ ] candidate measurements generated
[ ] comparison report generated
[ ] listening sanity check passed
[ ] ColdFire build passes
[ ] Digiemu test passes
[ ] hardware test passes
[ ] CPU budget recorded
[ ] provenance notes updated if needed
```

---

# 20. Reaper automation

Use one permanent Reaper project.

Suggested setup:

```text
Track 1: Gearmulator MD
Track 2: optional debug/click
```

A ReaScript should:

1. select the target machine
2. load the default reference patch
3. set one test parameter value
4. trigger the machine
5. offline-render the defined window
6. save with deterministic naming
7. continue to the next test point

Naming example:

```text
trx_b2__ptch_000.wav
trx_b2__ptch_016.wav
trx_b2__ptch_032.wav
```

The script should also support:

- interaction grids
- retrigger tests
- repeated stochastic hits

The Reaper project is shared infrastructure, not per-machine setup.

---

# 21. Agent handoff

Agents should normally receive:

```text
machine.yaml
measurements/
fitted behavior JSON
CHARACTERIZATION.md
existing DSP helpers
```

They should not need the complete raw WAV sweep unless they are investigating a specific uncertainty.

Good task:

```text
Implement TRX-B2 pitch and amplitude behavior from the measured
machine model using existing Digidrum helpers.

Deliver:
- trx_b2.c/.h
- host-render support
- tests
```

Another good task:

```text
Characterize TRX-SD SNAP and TONE behavior.

Deliver:
- reference sweeps
- compact measurements
- fitted parameter behavior
- CHARACTERIZATION.md update
```

Keep each task limited to one machine or one uncertain behavior.

---

# 22. First pipeline target

Use TRX-B2 to prove the complete system.

It exercises:

- pitch mapping
- pitch sweep
- hold + decay
- transient/noise
- bit reduction
- distortion
- retrigger behavior

Do not scale the process to many machines until TRX-B2 can go through:

```text
automated reference render
        ↓
measurement
        ↓
fitted model
        ↓
C implementation
        ↓
candidate comparison
        ↓
hardware validation
```

Once that works, the same pipeline can be reused across the Machinedrum catalog.
