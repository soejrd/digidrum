# Digidrum

**Bringing the original Machinedrum's sound machines to new hardware**

The goal is to recreate **every machine from the original Elektron Machinedrum** as a playable sound engine. Starting with the TRX kit, then PI, then working through the rest of the lineup. Each machine is being rebuilt from listening, measurements, published descriptions, and new DSP code. This is a recreation of the instruments and their behavior, not a copy or emulation of the original firmware.
<br/><br/>
<audio controls>
  <source src="website/demos/trx-01.mp3" type="audio/mpeg">
</a>
</audio>

[Sneak preview demo file](website/demos/trx-01.mp3)

[Try the TRX and EFM machines in your browser](https://soejrd.github.io/digidrum/).

This is a fresh era for these boxes. We can now explore new synthesis engines inside a familiar sequencer, with parameter locks, track effects, and hands-on controls.

## Machine progress

| Voice | Progress | Status |
| --- | --- | --- |
| TRX-BD | ⬜⬜⬜⬜⬜ | Not started |
| TRX-B2 | 🟩🟩🟩🟩🟨 | ~90% |
| TRX-SD | 🟨⬜⬜⬜⬜ | Init |
| TRX-XT | 🟨⬜⬜⬜⬜ | Init |
| TRX-CP | ⬜⬜⬜⬜⬜ | Not started |
| TRX-RS | 🟨⬜⬜⬜⬜ | Init |
| TRX-CB | 🟨⬜⬜⬜⬜ | Init |
| TRX-CH | 🟨⬜⬜⬜⬜ | Init |
| TRX-OH | 🟨⬜⬜⬜⬜ | Init |
| TRX-CY | 🟨⬜⬜⬜⬜ | Init |
| TRX-MA | 🟨⬜⬜⬜⬜ | Init |
| TRX-CL | 🟨⬜⬜⬜⬜ | Init |
| TRX-XC | 🟨⬜⬜⬜⬜ | Init |
| EFM-BD | 🟨⬜⬜⬜⬜ | Init |
| EFM-SD | 🟨⬜⬜⬜⬜ | Init |
| EFM-XT | 🟨⬜⬜⬜⬜ | Init |
| EFM-CP | 🟨⬜⬜⬜⬜ | Init |
| EFM-RS | 🟨⬜⬜⬜⬜ | Init |
| EFM-CB | 🟨⬜⬜⬜⬜ | Init |
| EFM-HH | 🟨⬜⬜⬜⬜ | Init |
| EFM-CY | 🟨⬜⬜⬜⬜ | Init |
| E12-BD | ⬜⬜⬜⬜⬜ | Not started |
| E12-SD | ⬜⬜⬜⬜⬜ | Not started |
| E12-HT | ⬜⬜⬜⬜⬜ | Not started |
| E12-LT | ⬜⬜⬜⬜⬜ | Not started |
| E12-CP | ⬜⬜⬜⬜⬜ | Not started |
| E12-RS | ⬜⬜⬜⬜⬜ | Not started |
| E12-CB | ⬜⬜⬜⬜⬜ | Not started |
| E12-CH | ⬜⬜⬜⬜⬜ | Not started |
| E12-OH | ⬜⬜⬜⬜⬜ | Not started |
| E12-RC | ⬜⬜⬜⬜⬜ | Not started |
| E12-CC | ⬜⬜⬜⬜⬜ | Not started |
| E12-BR | ⬜⬜⬜⬜⬜ | Not started |
| E12-TA | ⬜⬜⬜⬜⬜ | Not started |
| E12-TR | ⬜⬜⬜⬜⬜ | Not started |
| E12-SH | ⬜⬜⬜⬜⬜ | Not started |
| E12-BC | ⬜⬜⬜⬜⬜ | Not started |
| PI-BD | ⬜⬜⬜⬜⬜ | Not started |
| PI-SD | ⬜⬜⬜⬜⬜ | Not started |
| PI-XT | ⬜⬜⬜⬜⬜ | Not started |
| PI-RS | ⬜⬜⬜⬜⬜ | Not started |
| PI-ML | ⬜⬜⬜⬜⬜ | Not started |
| PI-MA | ⬜⬜⬜⬜⬜ | Not started |
| PI-HH | ⬜⬜⬜⬜⬜ | Not started |
| PI-RC | ⬜⬜⬜⬜⬜ | Not started |
| PI-CC | ⬜⬜⬜⬜⬜ | Not started |
| GND-SN | ⬜⬜⬜⬜⬜ | Not started |
| GND-NS | ⬜⬜⬜⬜⬜ | Not started |
| GND-IM | ⬜⬜⬜⬜⬜ | Not started |

## Super alpha: expect things to break

Digidrum is experimental firmware not ready for use. **Expect crashes, CPU overload, sound mismatches, missing features, and project or preset behavior that has not been fully verified.** The recent CH/OH prototypes caused fast crashes on a real Digitakt. Keep the stock OS update available and back up anything you care about before trying an alpha image.

Today, the firmware target is the **original Digitakt Mk1 running OS 1.53**. Digitone and Syntakt are part of the goal.

## How it is built

The machines are portable C99 using shared fixed-point oscillators, envelopes, noise, and filters. The Digitakt adapter renders into the stock track path, while the browser runs that same C code in an AudioWorklet. Each family has a C defaults table shared by both interfaces.

- [Architecture](Engineering/ARCHITECTURE.md)
- [DSP guide](Engineering/DSP_GUIDE.md)
- [Machine roadmap](Planning/MACHINE_ROADMAP.md)
- [Recreation pipeline](engineering/MACHINEDRUM_RECREATION_PIPELINE.md)
- [Testing and benchmarks](Verification/TESTING_AND_BENCHMARKS.md)
- [Project boundaries](Project%20definition/PROJECT_BOUNDARIES.md)
