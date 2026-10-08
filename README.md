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

| Voice | Progress |
| --- | --- |
| TRX-BD | 🟩🟩⬜⬜⬜ |
| TRX-B2 | 🟩🟩🟩🟩⬜ |  
| TRX-SD | 🟩🟩🟩🟩⬜ |   
| TRX-XT | 🟩🟩⬜⬜⬜ |   
| TRX-CP | 🟩🟩⬜⬜⬜ |   
| TRX-RS | 🟩🟩⬜⬜⬜ |   
| TRX-CB | 🟩🟩⬜⬜⬜ |   
| TRX-CH | 🟩🟩🟩⬜⬜ |
| TRX-OH | 🟩🟩🟩⬜⬜ |
| TRX-CY | 🟩🟩⬜⬜⬜ |   
| TRX-MA | 🟩🟩⬜⬜⬜ |   
| TRX-CL | 🟩🟩⬜⬜⬜ |   
| TRX-XC | 🟩🟩⬜⬜⬜ |   
| EFM-BD | 🟩🟩🟩🟩⬜ |   
| EFM-SD | 🟩🟩⬜⬜⬜ |   
| EFM-XT | 🟩🟩⬜⬜⬜ |   
| EFM-CP | 🟩🟩⬜⬜⬜ |   
| EFM-RS | 🟩🟩⬜⬜⬜ |   
| EFM-CB | 🟩🟩⬜⬜⬜ |   
| EFM-HH | 🟩🟩⬜⬜⬜ |   
| EFM-CY | 🟩🟩⬜⬜⬜ |   
| E12-BD | ⬜⬜⬜⬜⬜ |   
| E12-SD | ⬜⬜⬜⬜⬜ |   
| E12-HT | ⬜⬜⬜⬜⬜ |   
| E12-LT | ⬜⬜⬜⬜⬜ |   
| E12-CP | ⬜⬜⬜⬜⬜ |   
| E12-RS | ⬜⬜⬜⬜⬜ |   
| E12-CB | ⬜⬜⬜⬜⬜ |   
| E12-CH | ⬜⬜⬜⬜⬜ |   
| E12-OH | ⬜⬜⬜⬜⬜ | 
| E12-RC | ⬜⬜⬜⬜⬜ |    
| E12-CC | ⬜⬜⬜⬜⬜ |    
| E12-BR | ⬜⬜⬜⬜⬜ |    
| E12-TA | ⬜⬜⬜⬜⬜ |    
| E12-TR | ⬜⬜⬜⬜⬜ |    
| E12-SH | ⬜⬜⬜⬜⬜ |    
| E12-BC | ⬜⬜⬜⬜⬜ |    
| PI-BD | 🟩🟩⬜⬜⬜ |    
| PI-SD | 🟩🟩⬜⬜⬜ |    
| PI-XT | 🟩🟩⬜⬜⬜ |    
| PI-RS | 🟩🟩⬜⬜⬜ |    
| PI-ML | 🟩🟩⬜⬜⬜ |    
| PI-MA | 🟩🟩⬜⬜⬜ |    
| PI-HH | 🟩🟩⬜⬜⬜ |    
| PI-RC | 🟩🟩⬜⬜⬜ |    
| PI-CC | 🟩🟩⬜⬜⬜ |    
| GND-SN | 🟩🟩⬜⬜⬜ |    
| GND-NS | 🟩🟩⬜⬜⬜ |    
| GND-IM | 🟩🟩⬜⬜⬜ |    

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
- [Project boundaries](Project%20def ion/PROJECT_BOUNDARIES.md)
