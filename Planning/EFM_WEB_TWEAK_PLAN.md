# EFM Website Algorithm Tweak Plan

**Goal:** Add a Tweakpane-based editor to the browser (`/website/`) that exposes the hard-coded algorithm constants in `machines/efm.c`'s `update()` function as editable parameters, so EFM drum sounds can be matched to the original Machinedrum by adjusting pitch ranges, curves, mixes, envelope times, etc. Includes save/export/import and C-code patch generation.

## 1. Current state

- **`website/`** is a browser audition tool. It compiles `machines/efm.c` + `dsp/*.c` to WASM via `make web`, runs it in an `AudioWorklet`, and exposes the 8 front-panel Q7 knobs (0–127) per machine.
- **`website/trx_web.c`** is the C bridge. Currently exports: `dd_web_init`, `dd_web_set_control`, `dd_web_set_level`, `dd_web_trigger`, `dd_web_render`, `dd_web_capacity`, `dd_web_machine_count`, `dd_web_control_count`, `dd_web_default_control`.
- **`website/trx-worklet.js`** — AudioWorklet that instantiates WASM, handles step sequencing, forwards messages.
- **`website/script.js` + `index.html`** — UI with 8 knobs per machine, sequencer, tempo/level. Uses plain HTML/CSS sliders.
- **`machines/efm.c`** — the EFM synthesis. The `update()` function maps the 8 Q7 controls to internal parameters using **hard-coded constants** (min Hz, Hz ranges, ms ranges, depth multipliers, filter cutoffs, etc.). These constants are NOT exposed to the website.
- **GitHub Pages** — `.github/workflows/pages.yml` auto-deploys `website/` on push to `main`.

The synthesis C code is shared: browser, firmware, and tests all compile the same `efm.c`. Changes to the C voices are immediately audible in the browser.

## 2. What needs to change

### 2a. Parameter block (C side)

Create a `dd_efm_tweaks` struct that captures every hard-coded constant in `update()`, stored per-voice with a per-voice defaults table. Refactor `update()` to read from the struct instead of inline literals. The defaults preserve the current sound exactly.

**New file: `include/dd_efm_tweaks.h`** (or add to `dd_efm.h`):

```c
#include <stdint.h>
#include <stddef.h>

/* Every hard-coded constant currently inlined in efm.c update(). */
typedef struct {
    /* Pitch mapping: carrier_freq = c_min_hz + pitch_ctrl * c_hz_range / 127 */
    uint32_t c_min_hz;        /* carrier minimum (Hz)            e.g. BD 25,  SD 90  */
    uint32_t c_hz_range;      /* carrier range (Hz)             e.g. BD 95,  SD 350 */
    uint32_t m_min_hz;        /* modulator minimum (Hz)         e.g. BD 40,  SD 300 */
    uint32_t m_hz_range;      /* modulator range (Hz)           e.g. BD 1600, SD 2800 */

    /* Second FM pair (rimshot snare body) */
    uint32_t c2_min_hz;       /* second carrier min (Hz)       e.g. RS 80   */
    uint32_t c2_hz_range;     /* second carrier range (Hz)     e.g. RS 360  */
    uint32_t m2_offset_hz;    /* second modulator fixed offset e.g. RS 850  */
    uint32_t m2_range;        /* second modulator range        e.g. RS 10   */
    uint32_t rim_mod_ratio;   /* rim carrier/mod ratio mult   e.g. RS 2    */
    uint32_t rim_mod_offset;  /* rim mod fixed offset         e.g. RS 200  */

    /* Pitch sweep (BD, XT) */
    uint32_t sweep_max_hz;    /* max sweep rate                e.g. BD 600, XT 450 */

    /* Envelope decay time ranges (ms) */
    uint32_t amp_min_ms;      /* amplitude env min decay       e.g. 12 */
    uint32_t amp_span_ms;     /* amplitude env decay span       e.g. 1800 */
    uint32_t mod_min_ms;      /* modulator env min decay        e.g. 5  */
    uint32_t mod_span_ms;     /* modulator env decay span       e.g. 1200 */
    uint32_t ramp_min_ms;     /* ramp/pitch-sweep min           e.g. 4  */
    uint32_t ramp_span_ms;    /* ramp/pitch-sweep span          e.g. 500 */
    uint32_t aux_min_ms;      /* auxiliary env min (clap pre)   e.g. 3  */
    uint32_t aux_span_ms;     /* auxiliary env span             e.g. 50  */
    uint32_t mod_fixed_ms;    /* fixed mod env time (RS=65)     0=sweep  */
    uint32_t cb_aux_fixed_ms; /* CB aux fixed ms (8 + ctrl/7)   - */

    /* Modulation / feedback / mix multipliers */
    uint32_t depth_mult;      /* mod depth = ctrl[4] * depth_mult  e.g. BD 105 */
    uint32_t fb_depth_mult;   /* fb = ctrl * fb_depth_mult         e.g. BD 40, CB 55 */
    uint32_t fb_depth_fix;    /* fixed fb depth                    e.g. SD 1900, XT 2800 */
    uint32_t noise_gain_mult; /* noise mix = ctrl * noise_gain_mult e.g. SD 210, RS 258 */
    uint32_t snap_gain_mult;  /* snap/click mix = ctrl * snap_gain_mult */

    /* High-pass filter */
    uint32_t hp_min_hz;       /* HP cutoff min                   e.g. SD 30 */
    uint32_t hp_hz_range;     /* HP cutoff range                 e.g. SD 2500 */
    uint32_t hp_fixed_hz;     /* fixed HP cutoff                 e.g. HH 450 */
    uint32_t hp_frac_num;     /* HP as carrier/fraction num (XT) */
    uint32_t hp_frac_den;     /* HP as carrier/fraction den (XT) */

    /* Clap (CP) */
    uint32_t clap_max_count;  /* max pre-claps = ctrl[2] * clap_max_count/127 */
    uint32_t clap_period;     /* samples between pre-claps       e.g. 480 */

    /* Tremolo (HH) */
    uint32_t trem_depth_mult; /* trem depth = ctrl[2] * trem_depth_mult   e.g. HH 257 */
    uint32_t trem_freq_min_hz;/* tremolo min freq                e.g. HH 2 */
    uint32_t trem_freq_range; /* tremolo freq range              e.g. HH 68 */

    /* Inharmonic ratio table (HH, CY) */
    uint16_t ratio_0;         /* carrier/mod ratio 0 (x1000)     e.g. 1000 */
    uint16_t ratio_1;         /* ratio 1                         e.g. 1411 */
    uint16_t ratio_2;         /* ratio 2                         e.g. 1800 */
    uint16_t ratio_3;         /* ratio 3                         e.g. 2700 */

    /* Phase offset (XT carrier start phase) */
    uint8_t phase_offset_q2;  /* carrier start phase in units of pi/2 */

    /* CB second pair carrier/mod ratio */
    uint32_t cb_ratio_q4;     /* 1.48 -> 1480 */
} dd_efm_tweaks;
```

**Defaults table in `machines/efm.c`:**

```c
const dd_efm_tweaks dd_efm_default_tweaks[DD_EFM_MACHINE_COUNT] = {
    [DD_EFM_BD] = { .c_min_hz = 25, .c_hz_range = 95, .m_min_hz = 40, .m_hz_range = 1600,
                    .sweep_max_hz = 600, .amp_min_ms = 12, .amp_span_ms = 1800,
                    .mod_min_ms = 5, .mod_span_ms = 1200, .ramp_min_ms = 4, .ramp_span_ms = 500,
                    .depth_mult = 105, .fb_depth_mult = 40 },
    [DD_EFM_SD] = { .c_min_hz = 90, .c_hz_range = 350, .m_min_hz = 300, .m_hz_range = 2800,
                    .amp_min_ms = 12, .amp_span_ms = 1800, .mod_fixed_ms = 65,
                    .depth_mult = 95, .fb_depth_fix = 1900, .noise_gain_mult = 210,
                    .hp_min_hz = 30, .hp_hz_range = 2500, .aux_min_ms = 5, .aux_span_ms = 1100 },
    /* ... XT, CP, RS, CB, HH, CY with their respective constants ... */
};
```

**Voice struct** (`dd_efm.h`): add `dd_efm_tweaks tweaks;` field.

**`dd_efm_init()`**: copy defaults: `v->tweaks = dd_efm_default_tweaks[kind];`

**`update()` refactor**: replace each hard-coded literal with the corresponding `v->tweaks.xxx` field. No algorithmic change — only indirection through the parameter block.

### 2b. WASM bridge additions (`website/trx_web.c`)

Add a descriptor table and generic getter/setter. Uses `offsetof` for type-safe field access.

```c
#include <stddef.h>
#define TWEAK_COUNT 33

typedef struct {
    const char *name;
    uint32_t offset;
    uint32_t min, max;
} efm_tweak_desc;

static const efm_tweak_desc tweak_table[TWEAK_COUNT] = {
    {"carrier_min_hz",    offsetof(dd_efm_tweaks, c_min_hz),       0, 2000},
    {"carrier_hz_range",  offsetof(dd_efm_tweaks, c_hz_range),     0, 5000},
    {"modulator_min_hz",  offsetof(dd_efm_tweaks, m_min_hz),       0, 3000},
    {"modulator_hz_range",offsetof(dd_efm_tweaks, m_hz_range),     0, 5000},
    {"sweep_max_hz",      offsetof(dd_efm_tweaks, sweep_max_hz),   0, 2000},
    {"amp_min_ms",        offsetof(dd_efm_tweaks, amp_min_ms),     1, 10000},
    {"amp_span_ms",       offsetof(dd_efm_tweaks, amp_span_ms),    0, 10000},
    {"mod_min_ms",        offsetof(dd_efm_tweaks, mod_min_ms),     1, 10000},
    {"mod_span_ms",       offsetof(dd_efm_tweaks, mod_span_ms),    0, 10000},
    {"ramp_min_ms",       offsetof(dd_efm_tweaks, ramp_min_ms),    1, 1000},
    {"ramp_span_ms",      offsetof(dd_efm_tweaks, ramp_span_ms),   0, 1000},
    {"noise_gain_mult",   offsetof(dd_efm_tweaks, noise_gain_mult),0, 1000},
    {"snap_gain_mult",    offsetof(dd_efm_tweaks, snap_gain_mult), 0, 1000},
    {"depth_mult",        offsetof(dd_efm_tweaks, depth_mult),     0, 500},
    {"fb_depth_mult",     offsetof(dd_efm_tweaks, fb_depth_mult),  0, 500},
    {"fb_depth_fix",      offsetof(dd_efm_tweaks, fb_depth_fix),   0, 10000},
    {"hp_min_hz",         offsetof(dd_efm_tweaks, hp_min_hz),      0, 5000},
    {"hp_hz_range",       offsetof(dd_efm_tweaks, hp_hz_range),    0, 10000},
    {"hp_fixed_hz",       offsetof(dd_efm_tweaks, hp_fixed_hz),    0, 5000},
    {"trem_depth_mult",   offsetof(dd_efm_tweaks, trem_depth_mult),0, 1000},
    {"trem_freq_min_hz",  offsetof(dd_efm_tweaks, trem_freq_min_hz),0, 1000},
    {"trem_freq_range",   offsetof(dd_efm_tweaks, trem_freq_range),0, 5000},
    {"clap_max_count",    offsetof(dd_efm_tweaks, clap_max_count), 1, 10},
    {"clap_period",       offsetof(dd_efm_tweaks, clap_period),    1, 5000},
    {"ratio_0",           offsetof(dd_efm_tweaks, ratio_0),        1, 5000},
    {"ratio_1",           offsetof(dd_efm_tweaks, ratio_1),        1, 5000},
    {"ratio_2",           offsetof(dd_efm_tweaks, ratio_2),        1, 5000},
    {"ratio_3",           offsetof(dd_efm_tweaks, ratio_3),        1, 5000},
    {"phase_offset_q2",   offsetof(dd_efm_tweaks, phase_offset_q2),0, 4},
    {"c2_min_hz",         offsetof(dd_efm_tweaks, c2_min_hz),      0, 2000},
    {"c2_hz_range",       offsetof(dd_efm_tweaks, c2_hz_range),    0, 5000},
    {"m2_offset_hz",      offsetof(dd_efm_tweaks, m2_offset_hz),   0, 5000},
    {"cb_ratio_q4",       offsetof(dd_efm_tweaks, cb_ratio_q4),   1000, 2000},
};

uint32_t dd_web_efm_tweak_count(void);
uint32_t dd_web_efm_tweak_name(uint32_t index);  /* returns ptr into WASM mem */
uint32_t dd_web_efm_tweak_min(uint32_t index);
uint32_t dd_web_efm_tweak_max(uint32_t index);
uint32_t dd_web_efm_tweak_get(uint32_t index);
void dd_web_efm_tweak_set(uint32_t index, uint32_t value);
```

> **Note:** All 33 parameters are exposed for every EFM voice. Parameters unused by a given voice are simply ignored — they don't affect the sound. The UI can show/hide them per voice if desired, or just show all and let unused ones be harmless.

**Makefile**: add `--export` flags for all 6 new functions.

### 2c. Worklet updates (`website/trx-worklet.js`)

Add a message handler to pass tweak get/set through to the WASM instance:

```js
else if (message.type === 'tweak-get') {
    const value = this.wasm.dd_web_efm_tweak_get(message.index);
    this.port.postMessage({type: 'tweak-value', index: message.index, value});
}
else if (message.type === 'tweak-set') {
    this.wasm.dd_web_efm_tweak_set(message.index, message.value);
}
```

The worklet is the audio thread; reading strings and initializing the Tweakpane UI happens in the main thread (`script.js`), which sends messages to the worklet.

### 2d. UI: Tweakpane integration

**Dependencies:**
- Add Tweakpane via CDN (`<script src="https://unpkg.com/tweakpane@4.*"></script>`) in `index.html`
- No build step needed (vanilla JS, CDN load)

**UI layout:**
- Keep the existing 8 knobs for front-panel controls (matching the Machinedrum layout)
- Add a new collapsible **"Algorithm editor"** section below the knobs
- Inside: a Tweakpane pane with folders for each parameter group:
  - **Pitch** (carrier_min_hz, carrier_hz_range, modulator_min_hz, modulator_hz_range, sweep_max_hz, phase_offset)
  - **Envelopes** (amp, mod, ramp, aux decay ranges)
  - **Modulation** (depth_mult, fb_depth_mult, fb_depth_fix, noise_gain_mult, snap_gain_mult)
  - **Filter** (hp_min_hz, hp_hz_range, hp_fixed_hz)
  - **Special** (clap params, tremolo params, ratios, cb_ratio)

**Initialization flow:**
1. On WASM ready, fetch the parameter descriptor table (name, min, max, default for all 33 params)
2. Read per-voice current values from the voice struct (via `dd_web_efm_tweak_get`)
3. Build the Tweakpane pane dynamically from the descriptor table
4. On slider change, call `dd_web_efm_tweak_set(index, value)` and refresh the voice

**Save/Export:**
- **Export JSON** button: captures all 8 controls + all 33 tweak params as a `.json` file
- **Export C patch** button: generates a code snippet to update `dd_efm_default_tweaks[]` in `machines/efm.c`
- **Import JSON** button: loads a saved parameter set back into the UI and WASM voice

**JSON format:**
```json
{
  "version": 1,
  "machine": "EFM-BD",
  "kind": 8,
  "controls": [45, 65, 58, 48, 42, 35, 45, 25],
  "tweaks": {
    "carrier_min_hz": 25,
    "carrier_hz_range": 95,
    "modulator_min_hz": 40,
    "modulator_hz_range": 1600,
    ...
  }
}
```

**C patch output** (for pasting into `dd_efm_default_tweaks`):
```c
/* EFM-BD algorithm tweaks — export from browser */
[DD_EFM_BD] = {
    .c_min_hz = 22,
    .c_hz_range = 98,
    .m_min_hz = 38,
    .m_hz_range = 1550,
    .sweep_max_hz = 580,
    .amp_min_ms = 10,
    .amp_span_ms = 1700,
    .mod_min_ms = 4,
    .mod_span_ms = 1100,
    .ramp_min_ms = 3,
    .ramp_span_ms = 480,
    .depth_mult = 100,
    .fb_depth_mult = 38,
},
```

## 3. Files to modify

| File | Change |
|------|--------|
| `include/dd_efm.h` | Add `dd_efm_tweaks` struct, `tweaks` field in voice, declare `dd_efm_default_tweaks` |
| `machines/efm.c` | Add `dd_efm_default_tweaks[]` table; refactor `update()` to use `v->tweaks` instead of literals |
| `website/trx_web.c` | Add `dd_web_efm_tweak_*` bridge functions + descriptor table |
| `Makefile` | Add `--export` flags for 6 new WASM functions in the `web` target |
| `website/trx-worklet.js` | Add message handlers for tweak get/set/refresh |
| `website/index.html` | Add Tweakpane CDN script, algorithm editor panel |
| `website/script.js` | Add Tweakpane pane initialization, parameter enumeration, save/export/import |
| `website/style.css` | Add styling for algorithm editor panel |
| `website/README.md` | Document the new feature |

## 4. Implementation order

1. **Struct + defaults + refactor `update()`** — safest first step. The defaults table preserves current sound. Existing tests (`make test`, `make web`) must still pass / sound identical.
2. **WASM bridge functions** — add to `trx_web.c`, update Makefile exports.
3. **Worklet passthrough** — forward tweak messages.
4. **Tweakpane UI** — build the dynamic parameter panel, wire up save/export/import.
5. **Build + verify** — `make web`, serve locally, confirm all 8 EFM voices still sound correct with defaults, then start tweaking.

## 5. Testing/validation

- `make test` — existing C tests must pass (no algorithmic change)
- `make web` — WASM builds with new exports
- Local serve: `python3 -m http.server 8000 --directory website`, verify all EFM voices match current sound with defaults
- Verify tweak sliders actually change the audio in real-time
- Verify JSON export/import round-trips correctly
- Verify C patch output matches the `dd_efm_tweaks` struct field names
