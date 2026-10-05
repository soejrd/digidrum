# Agent / Developer Playbook

Use this document when handing a machine or infrastructure task to another developer or coding agent.

## 1. Rules for all tasks

1. Preserve stock Digitakt behavior outside the explicitly patched custom-machine path.
2. Write synthesis in C99.
3. Use fixed-point / integer DSP suitable for ColdFire.
4. Do not introduce 64-bit runtime helpers unless proven available and cheap.
5. Avoid expensive transcendental functions in inner loops.
6. Keep platform patch logic separate from synthesis code.
7. Add tests with every DSP primitive or machine.
8. Do not make hardware-performance claims from Digiemu alone.
9. Document any external algorithm inspiration and license implications.
10. Prefer a smaller algorithm that sounds good over a more exact but expensive model.
11. Do not remove sample/storage infrastructure merely because a sample UI path is hidden; trace dependencies and measure cost first.
12. Never modify the bootloader or compromise the stock recovery path.
13. Treat save/reload, p-locks, machine switching and stock-machine regressions as correctness requirements, not later polish.
14. Do not replace major stock-OS subsystems without a measured limitation and an explicit architecture decision.

## 2. Task template

Copy this into an issue / agent prompt:

```text
TASK:

Goal:

Files likely involved:

Existing behavior that must remain unchanged (including sequencing / persistence / stock-machine behavior):

DSP topology:

Seven synthesis parameters:
A =
B =
C =
D =
E =
F =
G =
H = LEVEL

Internal sample-rate divisor:

Required helpers:

Voice state:

Trigger/reset behavior:

CPU-risk areas:

Tests required:
- host deterministic render
- bounds / overflow
- 8-voice overlap
- cross-compile
- Digiemu smoke test
- p-lock test

Out of scope:

Persistence / workflow checks:

Definition of done:
```

## 3. Example task: TRX-B2-style machine

```text
TASK: Implement TRX-B2-style synthesized bass drum.

Goal:
Create a cheap bass-drum voice inspired by the behavioral shape of Elektron TRX-B2: swept body, hold/decay amplitude contour, click/noise transient and digital dirt.

DSP topology:
swept resonator -> amplitude hold/decay
+ click/noise transient
-> bit quantizer
-> soft/hard distortion

Parameters:
A PITCH
B HOLD
C TICK
D NOISE
E SWEEP
F DECAY
G DIRT
H LEVEL

Internal rate divisor:
2

Required helpers:
phase/resonator, decay envelope, hold-decay envelope, noise, quantizer, clipper, half-rate interpolator

Trigger behavior:
reset body phase/resonator and envelopes; noise RNG may remain free-running.

Tests:
verify hold duration changes; sweep direction/depth; DIRT quantization; 8 simultaneous voices bounded.

Out of scope:
exact Machinedrum waveform matching; firmware extraction; changes to stock filter/amp pages.
```

## 4. Review questions

Before merging a machine, ask:

- Can this reuse an existing helper instead of adding another custom DSP block?
- Is any coefficient recalculated per sample unnecessarily?
- Can this run at half or quarter rate?
- Is aliasing harmful here, or is it part of the sound?
- Are extreme parameter values musically useful and numerically safe?
- Does the machine remain distinct after the stock Digitakt filter/overdrive stage?
- Does the machine state fit comfortably per track?
- Can a contributor understand the topology from the source header alone?

## 5. Clean-room / provenance discipline

When approximating classic drum machines:

- implement from public descriptions, measured behavior and independently developed DSP ideas;
- do not copy proprietary firmware or disassembly-derived implementation code into the project;
- document which behaviors are observations vs inferred topology;
- keep third-party open-source code under compatible licenses and record attribution.

## 6. Branching work among agents

Good parallel work packages:

```text
Agent A: fixed-point + saturation helpers
Agent B: oscillators + lookup tables
Agent C: envelopes + multi-rate renderer
Agent D: resonator/filter primitives
Agent E: TRX-B2 machine
Agent F: EFM core
Agent G: test harness / golden renders
Agent H: hardware benchmark instrumentation
```

Avoid assigning two agents to modify the same firmware patch sites unless one owns integration.

## 7. Handoff format

Every completed task should report:

```text
Changed files:
New helpers:
State bytes per voice:
Internal sample rate:
Host tests:
Cross-build:
Digiemu tests:
Hardware tests:
Known issues:
Next suggested task:
```
