# Project Boundaries

This document is the project-level contract for Digidrum. Machine implementations and firmware patches should be reviewed against it.

## 1. Target and scope

- Target the original Digitakt Mk1.
- Initial firmware work is based on the verified OS 1.53 path.
- Digidrum replaces sample-based sound generation with synthesized drum machines; it is not an attempt to rewrite the whole Digitakt operating system.
- Digidrum is not a port of Machinedrum firmware. Machinedrum-like machines are clean-room approximations based on public documentation, measured behavior and independently implemented DSP.

## 2. Preserve the instrument around the synth engine

Preserve, unless a measured limitation requires otherwise:

- step and live pattern recording;
- trigs and retrigs;
- parameter locks;
- mutes and pattern changes;
- swing / accent and other useful sequencer behavior;
- stock downstream filter, overdrive, amp, sends and mixer where practical;
- settings, diagnostics and normal update / recovery behavior.

The synthesis engine should fit into the Digitakt workflow rather than forcing a new sequencer or mixer architecture prematurely.

## 3. Sample-system removal rule

Removing a user-facing browser or sample action does not prove that the underlying sample subsystem is unused or expensive.

Before removing any larger sample, storage or audio-recording service:

1. trace its dependencies;
2. determine whether project/sound serialization or sequencer code still relies on it;
3. measure its actual runtime cost;
4. verify stock-machine and recovery behavior;
5. remove or bypass only the smallest safe layer.

Do not delete infrastructure merely because its UI is hidden.

## 4. Boot and recovery rule

- Never modify the bootloader.
- Keep the normal firmware update and stock-firmware recovery path intact.
- Treat any change that risks recovery as out of scope for ordinary machine work.

## 5. Primary feasibility gate

The core go/no-go question is:

> Can the real Digitakt Mk1 run eight musically useful synthesized tracks simultaneously while keeping audio stable and the sequencer and controls responsive?

Digiemu is useful for correctness and regression testing, but it does not establish real hardware headroom. Until hardware measurements exist, eight-track operation and broad machine coverage remain targets, not claims.

## 6. Platform correctness before expansion

Before aggressively expanding the machine library, establish:

- machine selection and switching;
- storage of all synthesis parameters;
- parameter-lock behavior;
- project save/reload;
- sound/preset recall where applicable;
- copy/paste behavior;
- multi-track triggering;
- FILTER and AMP page behavior;
- stock-machine regression safety.

A large library of good-sounding voices is not useful if project state or sequencing semantics are unreliable.

## 7. Architecture escalation rule

Continue as a deep stock-OS modification while that architecture provides reliable sequencing, storage and hardware support. Replace larger firmware subsystems only when profiling or dependency analysis identifies a concrete limitation that cannot be solved cleanly within the current approach.

A standalone replacement OS is a separate milestone and should not emerge accidentally from individual machine work.
