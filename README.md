# Digidrum Synthesis Firmware

This directory is the implementation and handoff guide for turning the Digitakt Mk1 into a sample-free drum-synthesis platform.

The project already has a working sample-free machine spike: a custom machine can render synthesized audio into the stock Digitakt audio path, expose SRC-page parameters, accept parameter locks, and trigger on multiple tracks in Digiemu. Hardware performance and some persistence behavior are still unverified.

## Project boundaries

- Target the original Digitakt Mk1, initially around OS 1.53 and the currently verified patch path.
- Digidrum is a clean-room synthesis project, not a port of Machinedrum firmware. Similarity is judged by listening, measurement and parameter behavior rather than copied implementation.
- Replace sample generation with synthesis while preserving the useful stock Digitakt sequencer and downstream track path wherever practical.
- Preserve step/live recording, trigs, parameter locks, mutes, pattern changes, swing/accent, settings, diagnostics and normal firmware recovery behavior unless a measured reason justifies changing them.
- User-facing sampling features may eventually be hidden or removed, but underlying sample/storage services must not be removed until their dependencies and actual CPU/DSP cost are understood.
- Never modify the bootloader. Keep a reliable path back to stock firmware.
- Treat eight simultaneously useful synth tracks on real hardware, with responsive sequencing and controls, as the primary feasibility target. Until measured on hardware, track count and machine coverage are goals rather than promises.
- Continue as a deep modification of the stock OS while it provides reliable sequencing and hardware support. Replacing larger firmware subsystems requires a measured limitation and a separate design decision.

## Project goals

1. Run multiple synthesized drum voices on the original Digitakt Mk1 within ColdFire CPU limits.
2. Keep the stock Digitakt track pipeline where useful: filter, overdrive, amp, sends, mixer, sequencing and parameter locks.
3. Build reusable fixed-point DSP primitives rather than one-off machines.
4. Recreate the *behavior and character* of classic digital drum synthesis families, especially Machinedrum-like TRX / EFM / PI concepts, without requiring exact firmware cloning.
5. Make new instruments easy for agents and developers to implement, benchmark and integrate.

## Current verified baseline

- Target CPU: Motorola ColdFire 54455 / m68k.
- Compiler: `m68k-elf-gcc`.
- Instrument code: C99.
- Arithmetic: fixed-point, currently centered around Q15-style helpers.
- Render block: 32 samples.
- Custom machine audio can be injected before stock overdrive, after stock sample playback.
- SRC page can expose eight stored parameter slots; current instrument convention uses seven synthesis controls plus level.
- Parameter locks work for the tested custom parameter path in Digiemu.
- Two tracks can trigger the custom machine in the same audio block in Digiemu.
- Hardware performance is not yet established.

## Recommended document order

1. `ARCHITECTURE.md` — system boundaries, render pipeline and module layout.
2. `DSP_GUIDE.md` — fixed-point, multi-rate DSP, tables, envelopes and CPU rules.
3. `INSTRUMENT_API.md` — required structures, lifecycle and helper API.
4. `MACHINE_ROADMAP.md` — planned drum machines and implementation order.
5. `TESTING_AND_BENCHMARKS.md` — deterministic tests, quality gates and CPU profiling.
6. `AGENT_PLAYBOOK.md` — task format for agents / contributors.
7. `PROJECT_BOUNDARIES.md` — non-negotiable scope, safety and feasibility rules.

## Design principle

Prefer cheap, expressive primitives that can be reused across many machines:

- phase accumulators
- resonators
- recursive envelopes
- noise generators
- lookup-table oscillators
- one-pole filters
- fixed-point saturators / clippers
- sample-rate reduction and quantization as *intentional color*
- per-machine reduced-rate synthesis where safe

The architecture should optimize for **many cheap voices**, not one expensive general-purpose synthesizer.
