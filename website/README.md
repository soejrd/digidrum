# Browser machine test

The hosted test interface is available at <https://soejrd.github.io/digidrum/>. GitHub Pages publishes the committed `website/` files when they change on `main`. Audio starts after clicking **Trigger** or **Play**.

The page opens on EFM-BD with **Algorithm editor** expanded. Select any TRX or EFM machine to change its synthesis settings while listening. The TRX editor exposes pitch, decay time, sweep, noise level, and relevant source levels; its transient setting changes B2's tick level or the other voices' transient length. 100% preserves each machine's existing sound. The eight front-panel controls remain above the editor. Algorithm edits last for the current browser session; to make a change permanent, edit the C defaults and rebuild. The editor loads Tweakpane 4.0.5 from jsDelivr when opened, so that part needs network access.

EFM-BD follows the Figure 4.1 frequency path in `Resources/elektron-fm.md`: a decaying base frequency drives both the modulator (through the MFRQ ratio) and the carrier; the feedback modulator changes the carrier's instantaneous frequency, scaled by MOD and its envelope. DEC applies the final amplitude envelope. The ratio and maximum FM index are exposed as Q8 algorithm constants (256 means 1.0). The current BD defaults use the first Gearmulator listening values: carrier 12–402 Hz, sweep up to 2400 Hz, ratio 20–2420 in Q8, FM index up to 2048 in Q8, feedback multiplier 75, phase offset 2 quarters, and an 18 ms linear modulator attack before its decay. MFRQ uses a quadratic control mapping for finer low ratios. DEC and MDEC use the measured DEC anchors, and RDEC uses its separate measured anchors; RDEC duration has not been shortened. Table lookup and eight-sample decay updates remain fixed-point approximations for the target DSP.

To run it locally instead, from the `digidrum` root:

```sh
make web
python3 -m http.server 8000 --directory website
```

Open `http://localhost:8000`. Click **Trigger** for one hit, or activate sequencer steps and click **Play**. The page requests a 48 kHz `AudioContext`; it reports an error if the browser cannot provide one. Serve the files over localhost rather than opening `index.html` directly.

`trx-synth.wasm` is built with the local `emsdk` from `website/trx_web.c`, the `machines/*.c` voices, and shared `dsp/*.c` sources. Rebuild it after changing those sources. `trx-worklet.js` instantiates the WASM binary, keeps one C voice alive, runs the step clock, and writes stereo audio. `script.js` handles UI and sends control, machine, level, step, and transport messages. The selector exposes the eight TRX and eight EFM machines. This is a one-voice test interface, including when multiple steps retrigger it.

The EFM selector order is BD, SD, XT, CP, RS, CB, HH, CY. Their controls follow the manual order shown in the browser, including the seven-control CB and CY layouts. `machines/efm.c` ports the FM signal paths described by `Resources/elektron-fm.md` and the local `md-drum-synth` models into fixed-point 48 kHz C. The current defaults and response curves are provisional listening settings. The browser and firmware use the same synthesis source, so changes to the C voices can be auditioned here before hardware testing.

The prototypes use the architectures in `Resources/TRX.md` and `../md-drum-synth`, translated into fixed-point C using the shared oscillator, noise, and filter primitives. Control names shown in the browser describe the current mappings; controls marked `—` are reserved. The exact response curves, tuning, and levels have not been measured against an original Machinedrum. The new C module is also registered in the Digitakt firmware machine table, with hardware sound and performance still to be measured.

Selecting a machine loads its defaults from the single C table in `machines/trx_md.c`. The worklet reads this table from WASM and sends it to the UI; the Digitakt's fresh-machine parameter ranges use the same table. TRX-B2 starts at `PTCH 64, DEC 64, RAMP 64`, with its other five controls at `0`. The other machines currently have provisional defaults that can be adjusted after listening.

The current SD, CH/OH, CY, RS, and CL defaults and visible control counts incorporate listening comparisons. SD uses a 340 Hz lower oscillator whose PTCH control morphs from triangle to sine, and an upper sine oscillator tuned from 800 to 2000 Hz. BUMP/BENV affect only the lower oscillator. SNAP and TONE set the noise and upper-oscillator mix weights; all three sources share a normalized mix with about 6 dB of headroom. The noise has a 150 ms envelope, and DEC applies a curved overall decay to the complete mix before CLIP. CH/OH expose five controls in `GAP DEC HPF LPF MTAL` order, with narrower moving filter ranges. RS exposes `PTCH DEC DIST`; its short click stays fixed while DIST frequency-modulates the decaying tone. CL exposes `PTCH DEC DUAL ENH TUNE CLIC`; TUNE moves only the second tone and CLIC is deliberately quiet. CY keeps the existing sound with the reported reference settings. CB is unchanged pending a later comparison. These are listening-guided approximations; parameter sweeps are still needed for the exact mappings.

CH/OH render their six-square source and eight filter stages at 24 kHz, then interpolate to the 48 kHz output. Oscillator rates and filter coefficients update only when controls change; the filter stages use the shared one-multiply one-pole helpers. All prototype voices skip synthesis after their envelopes end. This reduces DSP cost, but hardware CPU headroom and the resulting high-frequency timbre need another listening and digihealth check.

### What the TRX paper specifies

- **BD:** one sine-like oscillator, pitch drop controlled by RAMP/RDEC, and a short sampled attack blended with the tone. The prototype synthesizes its attack because the paper does not supply that sample.
- **SD:** two tones whose frequency interval stays constant during the pitch bump, plus filtered snare noise. The paper lists `PTCH DEC SNAP NOIS TONE TUNE BUMP CLIP`, while the current browser layout follows the eight controls observed in Gearmulator: `PTCH DEC BUMP BENV SNAP TONE TUNE CLIP`. The exact relationship between these versions needs measurement.
- **CH/OH:** six fixed square-like oscillators mixed with local noise. The paper describes a controllable 24 dB/oct low-pass and two 12 dB/oct high-pass filters, one fixed and one controllable, with GAP as an envelope hold before decay. The prototype uses four low-pass and four high-pass one-pole stages and the hold/decay envelope. Its paper lists six controls including PEAK, while the tested Gearmulator version exposes five.
- **CY/RS/CB/CL:** the paper identifies their synthesis groups but does not give comparable per-machine DSP code or control curves. Their current details are still inferred from the documented group structure and the other local source.

The OCR of the paper's six square-wave period table is internally inconsistent: it associates period `114` with approximately `306 Hz` at `44.1 kHz`, although direct division gives about `387 Hz`. The current frequency set is therefore provisional rather than presented as an exact transcription. The original DSP uses a larger, higher-precision sine table; these prototypes reuse this project's shared fixed-point table.

Run the focused worklet check with:

```sh
node tests/test_web_audio.mjs
```

To add a different machine family later, extend the C bridge and the worklet's machine selection. Keep synthesis in the shared C source rather than adding a JavaScript copy.
