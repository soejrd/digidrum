# Digidrum

**Bringing the original Machinedrum's sound machines to new hardware**

The goal is to recreate **every machine from the original Elektron Machinedrum** as a playable sound engine. Starting with the TRX kit, then PI, then working through the rest of the lineup. Each machine is being rebuilt from listening, measurements, published descriptions, and new DSP code. This is a recreation of the instruments and their behavior, not a copy or emulation of the original firmware.
<br/><br/>
<audio controls>
  <source src="website/demos/trx-01.mp3" type="audio/mpeg">
</a>
</audio>
<br/>

[Sneak preview demo file](website/demos/trx-01.mp3)

This is a fresh era for these boxes. We can now explore new synthesis engines inside a familiar sequencer, with parameter locks, track effects, and hands-on controls. The current TRX voices are an early glimpse of what a complete drum-synthesis instrument could become.

## Super alpha: expect things to break

Digidrum is experimental firmware. **Expect crashes, CPU overload, sound mismatches, missing features, and project or preset behavior that has not been fully verified.** The recent CH/OH prototypes caused fast crashes on a real Digitakt; an optimized build is available, but its hardware stability still needs testing. Keep the stock OS update available and back up anything you care about before trying an alpha image.

Today, the firmware target is the **original Digitakt Mk1 running OS 1.53**. Digitone support is part of the goal, but there is no Digitone build yet. The current SysEx is not for Digitakt II.

## How it is built

The machines are portable C99 using shared fixed-point oscillators, envelopes, noise, and filters. The Digitakt adapter renders into the stock track path, while the browser runs that same C code in an AudioWorklet. Machine defaults live in one C table and feed both interfaces.

- [Architecture](Engineering/ARCHITECTURE.md)
- [DSP guide](Engineering/DSP_GUIDE.md)
- [Machine roadmap](Planning/MACHINE_ROADMAP.md)
- [Recreation pipeline](engineering/MACHINEDRUM_RECREATION_PIPELINE.md)
- [Testing and benchmarks](Verification/TESTING_AND_BENCHMARKS.md)
- [Project boundaries](Project%20definition/PROJECT_BOUNDARIES.md)