# Browser machine test

From the `digidrum` root:

```sh
make web
python3 -m http.server 8000 --directory website
```

Open `http://localhost:8000`. Click **Trigger** for one hit, or activate sequencer steps and click **Play**. The page requests a 48 kHz `AudioContext`; it reports an error if the browser cannot provide one. Serve the files over localhost rather than opening `index.html` directly.

`trx-synth.wasm` is built with the local `emsdk` from `website/trx_web.c` plus the same `machines/trx_md.c` and `dsp/*.c` sources used by firmware. Rebuild it after changing those sources. `trx-worklet.js` instantiates the WASM binary, keeps one C voice alive, runs the step clock, and writes stereo audio. `script.js` handles UI and sends control, machine, level, step, and transport messages. The current selector exposes TRX-BD, TRX-B2, and TRX-SD through the same bridge; TRX-B2 is the default. This is a one-voice test interface, including when multiple steps retrigger it.

Run the focused worklet check with:

```sh
node tests/test_web_audio.mjs
```

To add a different machine family later, extend the C bridge and the worklet's machine selection. Keep synthesis in the shared C source rather than adding a JavaScript copy.
