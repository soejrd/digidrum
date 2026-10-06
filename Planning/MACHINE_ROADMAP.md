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

Exit criterion: a custom synthesized machine behaves like a reliable Digitakt machine across editing, p-locking, sequencing and persistence, with a repeatable hardware profiling path.

## Phase 0B — shared DSP foundation

Build these primitives in parallel once the integration contract is stable:

- [x] fixed-point math helpers
- [x] saturating arithmetic
- [x] 32-bit phase accumulator
- [x] sine lookup oscillator
- [x] triangle / square oscillator
- [x] recursive decay envelope
- [x] attack-hold-decay envelope
- [x] pitch sweep envelope
- [x] xorshift noise
- [x] one-pole LP / HP
- [x] damped 2-pole resonator
- [x] hard / soft clip
- [x] bit quantizer
- [x] half-rate / quarter-rate selection
- [x] zero-order hold + linear interpolation
- [x] block-level parameter coefficient cache

Exit criterion: primitives cross-compile, have deterministic tests, and can render a synthetic benchmark voice.

The shared parameter cache tracks changes once per render block. The benchmark voice uses those changes to refresh its derived settings, including decay coefficient and oscillator increment. Host tests, both ColdFire cross-checks, and the synthetic WAV render pass. The benchmark is a deterministic DSP exercise; it is not a hardware timing or audio-quality measurement.

## Phase 1 — TRX-style core family

Portable TRX-B2 and TRX-SD voices now render at half rate with the
eight machine controls from the Machinedrum manual, plus a separate track level.
The control names and order follow the [Elektron manual](https://www.elektron.se/wp-content/uploads/2024/09/machinedrum_manual_OS1.63.pdf);
the DSP algorithms are original behavioral approximations.
They pass deterministic, bounds, control-response and eight-voice host tests,
and cross-compile for ColdFire. WAV demos are available via `make demo`.
They are registered as Digitakt machine IDs 10–11, using all eight SRC slots;
machine IDs 8–9 are now unregistered, while the remaining TRX and EFM machines retain IDs 10–25. The SDK built,
linted and patched the combined firmware. Digiemu cold boot is currently blocked by a native Unicorn
`Illegal instruction` on this host, so UI/audio behavior remains unverified;
Phase 0A platform correctness remains a separate gate.

### TRX-B2-inspired bass drum

Priority: **P0**

Topology:

```text
swept sine body
  + click transient
  + short filtered noise
  -> hold + decay amplitude contour
  -> bit dirt
  -> distortion
```

Eight synth controls (level remains a separate track control):

```text
PTCH / DEC / RAMP / HOLD / TICK / NOIS / DIRT / DIST
```

DIRT reduces bit depth; DIST controls the voice's separate distortion stage.

Internal rate: HALF.

Why first: expressive, cheap, iconic behavior, and a good stress test for pitch envelopes + nonlinear digital color.

### TRX-BD-inspired bass drum (deferred)

Priority: **P0**

Topology:

```text
sine body
  <- fast start boost + independent pitch ramp
+ short noise/transient
  + second harmonic oscillator
-> clip
```

Eight synth controls (level remains a separate track control):

```text
PTCH / DEC / RAMP / RDEC / STRT / NOIS / HARM / CLIP
```

Internal rate: HALF.

### TRX-XT / tom / percussion body

Priority: **P1**

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

Priority: **P0**

Topology:

```text
2 tonal oscillators
  <- transient pitch bump
+ filtered noise snap
-> clip
```

Eight synth controls (level remains a separate track control):

```text
PTCH / DEC / BUMP / BENV / SNAP / TONE / TUNE / CLIP
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
2. TRX-BD
3. TRX-B2
4. TRX-SD
5. TRX-XT
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
