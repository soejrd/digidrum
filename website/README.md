# Browser machine test

**The web player is temporarily offline.** The `website/` page now shows a
placeholder screen ("coming next week") while the machine browser is reworked.
There is no interactive player and no audio to load from the page at the moment.

## Current state

- `website/index.html` is a holding screen only. It has no controls, no WASM
  load, and no AudioWorklet.
- `website/script.js`, `website/trx-worklet.js`, `website/algorithm-editor.js`,
  `website/trx_web.c` and `website/trx-synth.wasm` are left in the tree but are
  not referenced by the page, so they are dormant. They can be reattached when
  the player returns.
- The committed `trx-synth.wasm` is a stale build from the previous player and is
  not loaded.

## When the player returns

Rebuild the WASM from the shared C voices, then restore the player markup and
scripts. From the `digidrum` root:

```sh
make web
python3 -m http.server 8000 --directory website
```

`make web` builds `website/trx-synth.wasm` from `website/trx_web.c`, the
`machines/*.c` voices, and shared `dsp/*.c` sources. The worklet instantiates the
WASM, keeps one C voice alive, runs the step clock, and writes stereo audio;
`script.js` handles UI and transport messages. Serve over localhost rather than
opening `index.html` directly.

## History (previous player)

The text below describes the player that was removed on 2026-10-09, kept for
when it is restored.

The hosted test interface was published at <https://soejrd.github.io/digidrum/>.
The page opened on EFM-BD with the **Algorithm editor** expanded, and offered the
eight TRX and eight EFM machines with their front-panel controls, a trigger, and
a 16-step sequencer. The editor loaded Tweakpane 4.0.5 from jsDelivr, so that
part needed network access. Algorithm edits lasted for the browser session only;
to make a change permanent, edit the C defaults and rebuild.

The EFM selector order was BD, SD, XT, CP, RS, CB, HH, CY. Their controls
followed the manual order, including the seven-control CB and CY layouts.
`machines/efm.c` ported the FM signal paths from `Resources/elektron-fm.md` into
fixed-point 48 kHz C, and the defaults and response curves were provisional
listening settings.

Selecting a machine loaded its defaults from the single C table in
`machines/trx_md.c`. The worklet read this table from WASM and sent it to the UI.

Run the focused worklet check with `node tests/test_web_audio.mjs` (it exercises
`trx-worklet.js` and `trx-synth.wasm` directly and does not need the page).
