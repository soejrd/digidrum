# Testing and Performance Plan

## 1. Current known status

The existing spike has already demonstrated in Digiemu:

- audible synthesized PULSE BD output;
- live editing of the previously problematic SRC D slot;
- parameter-lock recall for that slot;
- two custom-machine tracks triggering in the same block;
- stock SRC D browser behavior remains intact on stock machines.

Still open:

- project save/reload persistence;
- real hardware CPU and audio behavior;
- real eight-track synthesis headroom;
- cost of the stock sample path and whether disabling parts of it frees useful CPU/DSP capacity.

## 2. Primary feasibility gate

The project-level performance acceptance test is real-hardware operation of eight musically useful synthesized tracks while audio remains stable and the sequencer/UI remain responsive. Emulator timing is not sufficient evidence for this claim.

Record both average and worst-block cost, especially simultaneous triggers and parameter-lock changes. If eight tracks do not fit, use measurements to decide whether to simplify machines, reduce internal rates, reclaim stock sample-path work or revise the track target.

## 3. Unit-test categories

### Math

- multiply edge cases
- saturating add/sub
- interpolation
- clip curves
- quantization

### Oscillator

- phase wrap
- deterministic lookup
- frequency mapping
- reduced-rate interpolation

### Envelope

- monotonic decay
- hold duration
- trigger reset
- no underflow/overflow runaway

### Resonator

- bounded output for legal coefficients
- expected decay
- impulse response deterministic

### Noise

- deterministic seeded sequence
- full-range output
- no stuck states

## 4. Machine tests

Every machine gets:

```text
silence test
single-trigger render
retrigger test
extreme parameter combinations
parameter change during active voice
8 simultaneous independent voices
long-run boundedness
```

Useful numeric assertions:

- peak output <= chosen safe bound;
- late-block energy < early-block energy for decaying voices;
- no voice state overflows / wraps unexpectedly;
- repeated seeded renders are bit-identical.

## 5. Golden renders

For each machine, keep a tiny set of deterministic raw or WAV renders:

```text
default.wav
short.wav
long.wav
extreme.wav
```

Golden renders are not for exact sound quality approval. They catch accidental DSP changes.

Where storage is a concern, store hashes plus small analysis summaries instead of full audio.

## 6. CPU benchmark harness

Create a host-side operation / timing harness and a hardware-side timing probe.

Benchmark cases:

```text
1 voice sustained
4 voices sustained
8 voices sustained
8 voices simultaneous trigger
8 voices with parameter changes
worst-case machine mix
```

Report:

```text
machine
internal rate divisor
cycles or timer delta per 32-sample block
peak block cost
steady-state cost
voice state bytes
code bytes
```

## 7. Hardware profiling gate

Digiemu CPU estimates are useful for regression direction only. Do not claim hardware headroom from emulator values.

Before enabling many machines by default, collect real hardware measurements for:

- stock project baseline;
- one custom synth voice;
- 4 voices;
- 8 voices;
- 8 voices + stock filter/overdrive/sends;
- worst transient block where all voices trigger simultaneously.

## 7. Audio quality tests

Subjective evaluation should cover:

- low-frequency stability
- aliasing character
- transient sharpness
- p-lock zippering
- retrigger consistency
- extreme-control usefulness
- interaction with stock overdrive/filter

Do not automatically “fix” aliasing if it contributes positively to the intended machine.

## 9. Persistence / integration tests

Before calling the machine framework stable:

- verify project save/reload of machine IDs;
- verify all seven parameters survive project save/reload;
- verify sound pool recall;
- verify pattern locks after reload;
- verify stock sample machines remain unaffected;
- verify machine switching does not leave stale synthesis state;
- verify copy/paste behavior;
- verify pattern save/load and normal recording workflows;
- verify FILTER and AMP page mappings before repurposing their controls;
- verify settings, diagnostics and recovery/update behavior remain intact.

## 10. Regression checklist

For every firmware build:

- [ ] host tests
- [ ] cross-check
- [ ] lint / patch overlap check
- [ ] firmware repack verification
- [ ] Digiemu boot
- [ ] custom machine trigger
- [ ] stock sample-machine trigger
- [ ] SRC parameter edit
- [ ] p-lock
- [ ] two-track simultaneous trigger
- [ ] no browser regression on stock SRC D
