# Drum Machine Roadmap

The goal is not byte-identical Machinedrum emulation. The goal is a compact family of machines with similar synthesis behavior, parameter response and sonic range, implemented from reusable primitives.

## Phase 0A — platform correctness

Before expanding the synthesis library, close the workflow and persistence risks exposed by the first spike.

- [*] verify machine selection and switching across tracks
- [*] verify all seven synthesis parameters plus level remain stored and lockable
- [ ] verify project save/reload preserves machine IDs and parameter values
- [ ] verify sound/preset recall where applicable
- [ ] verify copy/paste and pattern workflows
- [ ] verify multi-track simultaneous triggering beyond the two-track smoke test
- [ ] map and verify machine-aware FILTER and AMP behavior

Exit criterion: a custom synthesized machine behaves like a reliable Digitakt machine across editing, p-locking, sequencing and persistence, with a repeatable hardware profiling path.

## Phase 0B — shared DSP foundation

Build these primitives in parallel once the integration contract is stable:

- [ ] fixed-point math helpers
- [ ] saturating arithmetic
- [ ] 32-bit phase accumulator
- [ ] sine lookup oscillator
- [ ] triangle / square oscillator
- [ ] recursive decay envelope
- [ ] attack-hold-decay envelope
- [ ] pitch sweep envelope
- [ ] xorshift noise
- [ ] one-pole LP / HP
- [ ] damped 2-pole resonator
- [ ] hard / soft clip
- [ ] bit quantizer
- [ ] half-rate / quarter-rate renderer
- [ ] zero-order hold + linear interpolation
- [ ] block-level parameter coefficient cache

Exit criterion: primitives cross-compile, have deterministic tests, and can render a synthetic benchmark voice.

## Phase 1 — TRX-style core family

### TRX-B2-inspired bass drum

Priority: **P0**

Topology:

```text
swept sine/resonator
  + click transient
  + short filtered noise
  -> hold + decay amplitude contour
  -> bit dirt
  -> distortion
```

Seven controls:

```text
PITCH / HOLD / TICK / NOISE / SWEEP / DECAY / DIRT
```

Optional distortion amount can be coupled to DIRT or moved to the stock track overdrive.

Internal rate: HALF.

Why first: expressive, cheap, iconic behavior, and a good stress test for pitch envelopes + nonlinear digital color.

### TRX-BD-inspired bass drum

Priority: **P0**

Topology:

```text
resonant body
  <- two-stage pitch sweep
+ short noise/transient
-> harmonic shaping
-> clip
```

Controls:

```text
PITCH / HARM / ATTACK / PUNCH / SWEEP / DECAY / CLIP
```

Internal rate: HALF.

### TRX-XT / tom / percussion body

Priority: **P0**

Topology:

```text
impulse -> resonator <- pitch envelope -> damping -> distortion
```

Controls:

```text
PITCH / BODY / DAMP / ATTACK / SWEEP / DECAY / DIST
```

Internal rate: HALF.

### TRX-SD-inspired snare

Priority: **P1**

Topology:

```text
2 body resonators
+ transient pitch bump
+ filtered noise
-> clip
```

Controls:

```text
PITCH / TUNE / SNAP / TONE / BUMP / DECAY / CLIP
```

Internal rate: HALF.

### TRX-HH metallic hat

Priority: **P1**

Topology:

```text
4-6 inharmonic square/triangle oscillators
-> sum
-> HP/BP shaping
-> envelope
-> clip
```

Controls:

```text
PITCH / METAL / GAP / COLOR / ATTACK / DECAY / GRIT
```

Internal rate: QUARTER or HALF.

## Phase 2 — EFM family

Build a reusable 2-operator FM core with optional feedback.

Core:

```text
modulator + feedback
    -> decaying modulation index
    -> carrier phase modulation
    -> amplitude envelope
```

### EFM-BD

Priority: **P1**

```text
PITCH / RATIO / INDEX / FEEDBACK / SWEEP / DECAY / DRIVE
```

### EFM-SD

Priority: **P1**

FM body + noise/snare layer.

```text
PITCH / RATIO / INDEX / NOISE / TONE / DECAY / FEEDBACK
```

### EFM-TOM / rim / cowbell

Priority: **P2**

Reuse the same FM core with different ratio maps, transient envelopes and optional second mode.

## Phase 3 — physical / modal family

### PI-BD / PI-XT

Priority: **P2**

Use 2-4 resonant modes excited by an impulse or short noise burst.

```text
PITCH / MATERIAL / STRIKE / SPREAD / TENSION / DECAY / DRIVE
```

Internal rate: HALF.

### Metallic modal percussion

Priority: **P2**

Mode ratios move from harmonic to prime/Fibonacci/inharmonic sets.

```text
PITCH / MATERIAL / RATIOS / SPREAD / STRIKE / DECAY / CHAOS
```

## Phase 4 — original Digidrum machines

These are not Machinedrum approximations; they exploit the same infrastructure.

### METAL CLUSTER

```text
4-8 oscillators
-> ratio set morph
-> FM/crossmod
-> ring modulation / fold
-> decay
```

Controls:

```text
PITCH / RATIOS / SPREAD / FM / SWEEP / DECAY / GRIT
```

### DIGITAL DEBRIS

```text
integer / random-walk generator
-> clocked hold
-> bit quantizer
-> resonator
```

Controls:

```text
RATE / ALGO / STATE / MOD / BITS / DECAY / RESO
```

### RUNG DRUM

Stateful stepped oscillator that may partially reset on trigger.

```text
PITCH / STATE / RATE / FEEDBACK / RESET / DECAY / GRIT
```

## Suggested implementation sequence

```text
0. platform correctness + hardware baseline
1. shared primitives
2. TRX-B2
3. TRX-BD
4. TRX-XT
5. TRX-SD
6. EFM core
7. EFM-BD
8. EFM-SD
9. TRX-HH
10. PI modal core
11. PI-BD / PI-XT
12. METAL CLUSTER
13. DIGITAL DEBRIS
```

This order maximizes primitive reuse while giving useful instruments early.

## Definition of done for each machine

- [ ] seven synth controls have documented semantic ranges
- [ ] level handled separately
- [ ] no expensive per-sample transcendental functions
- [ ] internal sample-rate divisor chosen intentionally
- [ ] no overflow in normal parameter range
- [ ] deterministic host render test
- [ ] 8-voice overlap host test
- [ ] cross-build passes with `-Werror`
- [ ] Digiemu can select / edit / p-lock / trigger machine
- [ ] sound remains bounded under extreme p-lock changes
- [ ] hardware CPU measurement recorded when available
- [ ] provenance / inspiration documented
