# Machinedrum-style synthesis drums — design & implementation plan

Goal: a family of dgenlisp drum instruments with the character of the Elektron
Machinedrum SPS-1's synthesis machines (TRX / EFM / P-I / GND) — cheap DSP,
enormous sound. Not a clone of any one machine; the same philosophy: few
oscillators, exponential envelopes, aggressive nonlinearity, 8 opinionated
parameters per engine where every knob is musical.

This replaces the failed `drums/3d-drum`, `drums/3d-snareo`, and
`drums/machinedrum-physical` attempts.

---

## 1. Why the previous attempts were boring (design principles)

Diagnosis of `3d-drum` / `3d-snareo`:

1. **Additive organ syndrome.** Many static inharmonic sine "modes," each with
   its own slow ADSR, summed. Result: a steady phasey "bonggg," not a drum.
   A drum is one dominant pitched body + a violent transient + noise — three
   things with *very* different time constants.
2. **Soft transients.** The shared `adsr` macro fades retriggers through a 3 ms
   de-click window and ramps attack linearly. Drum machines hit at sample zero.
3. **Linear/uniform envelope shapes.** All character in analog drums lives in
   exponential decay curves and pitch envelopes that are 5–20× faster than the
   amp envelope.
4. **Nonlinearity only at the end.** One `tanh` on a sum of 12 partials makes
   intermodulation mush. The 808/909/MD put clipping *inside* the voice (pulse
   shapers, swing-VCAs, per-band clippers) where it creates harmonics, not mud.
5. **Every hit identical.** No per-trigger randomness, velocity only scales
   gain. Real circuits have ±20 % component tolerance, energy-dependent pitch,
   stochastic noise — hits breathe.
6. **Parameter sprawl.** 30+ overlapping knobs (3d-snareo) means no opinion.
   The MD gives each machine ≤ 8 params, each with a large audible range.
7. **Fake rooms and pulse-train "rattles."** Periodic `pow(sin)` pulse trains
   read as buzzing LFOs. Rattle is stochastic (PhISEM, §6.4); "room" is the
   808 clap's single 100 ms noise tail, not a bank of sine "room modes."

Every engine below is designed against these seven points.

---

## 2. What the Machinedrum actually is (research summary)

Full research with sources at the bottom. Key facts:

- Hardware: 2× Motorola DSP56303 (24-bit fixed point, 100 MMACS) — one renders
  all 16 voices (~130 instructions/sample budget per voice!), one does mixing +
  track FX. Engine rate ≈ 44.1 kHz; UW sample playback is 12-bit (the famous
  grit). The synthesis machines are *that* cheap — our budget is luxurious.
- Four synthesis families:
  - **TRX** — "inspired by classic analogue drum machine synthesis" (606/808/909
    style): swept sine/bridged-T voices, 6-square-oscillator metal cluster for
    hats/cymbals, retriggered-noise clap, all with waveshaper/clip stages.
  - **EFM** — "Enhanced Feedback Modulation": 2-op FM, modulator at an
    *absolute* frequency (not a ratio), per-machine auxiliary exciters, and
    modulator self-feedback that tips from gritty into chaotic/noisy regimes
    (the hat/cymbal "noise" is feedback FM, no noise generator).
  - **P-I** — "Physically Informed" — Perry Cook's PhISM/PhISEM lineage:
    modal resonator banks + shaped mallet excitation + tension-modulated pitch
    glide + stochastic grain layers. PI-MA is a textbook PhISEM shaker.
  - **GND** — raw tools: sine w/ pitch ramp, raw noise, programmable impulse.
- Per-track FX chain (a huge part of "the MD sound"), in order:
  synthesis → **AM** (tremolo LFO, "can go very high in frequency") → **EQ**
  (1-band) → **filter** (24 dB resonant; FLTF = HP edge, FLTW = gap width to
  the LP edge, FLTQ boosts *both* edges) → **SRR** (sample-rate reduction) →
  **DIST** ("applied after the synthesis and track effects").

---

## 3. dgenlisp capability inventory (all confirmed present)

| Need | Primitive | Notes |
|---|---|---|
| Exponential envelopes, resonators, FM feedback | scalar `make-history`/`read-history`/`write-history` | used throughout `INSTRUMENT_PREAMBLE` (svf/ladder/adsr) |
| Variable interpolated delay (KS wires, combs) | `(delay sig samples)` | 88 000-sample circular buffer, persistent, write-before-read |
| Filters | `svf` (ZDF; LP/BP/HP/notch/peak/AP), `ladder`, `biquad` | track-filter = cascaded svf pairs |
| Per-hit randomness | `(latch (noise) trigger)` | per-note constants — drift instrument proved the pattern |
| Per-sample stochastic events | `noise` + comparisons + `gswitch` | PhISEM collisions |
| Impulse/trigger | `click`, `ramp2trig`, `trigger` input | |
| Branchless engine switching | `selector`, `gswitch` | operator's 11 FM algorithms prove the pattern |
| Resettable phasor / burst counters | `accum` with reset (workflow memory trick) | clap burst trains, retrig envelopes |
| Anti-aliased squares | `polyblep_pulse` | hat oscillator bank |

Gotchas to respect (from memory):
- `(mod param)` does not resolve through macro substitution — read `@mod true`
  params at top level, pass values into macros.
- `write-history` return value is miswired — write as a bare statement, consume
  the value expression (`(def next …) (write-history h next)`).
- Instrument contract: `dsp.lisp` + `ui.lisp` folder under
  `content/instruments/`, inputs gate/pitch/vel/trigger/clock at 1–5,
  mod1–4 at 6–9; spec in `crates/sequencer/src/agent/prompts/instrument.md`.

---

## 4. Instrument lineup

One instrument per *drum role* (how people use tracks), each with 3–5 engines
switched by an `engine` option param (branchless; all engines always compute —
each is a handful of oscillators, trivially affordable). Engine params follow
the MD's names where possible; engine-specific panels in the UI (operator-style
mode-driven center).

| Folder | Engines | MD machines covered |
|---|---|---|
| `drums/md-kick` | TRX-BD, TRX-B2, EFM-BD, PI-BD, GND-SN | 5 |
| `drums/md-snare` | TRX-SD, EFM-SD, PI-SD, + RIM sub-engines (TRX-RS, EFM-RS, PI-RS) | 6 |
| `drums/md-hat` | TRX-HH (CH/OH continuum), EFM-HH, PI-HH | 4 |
| `drums/md-cymbal` | TRX-CY, EFM-CY, PI-RC, PI-CC | 4 |
| `drums/md-tom` | TRX-XT, TRX-XC, EFM-XT, PI-XT | 4 |
| `drums/md-perc` | TRX-CP, TRX-CB, TRX-CL, TRX-MA, EFM-CB, PI-ML, PI-MA | 7 |

(md-perc may split into md-clap + md-perc during implementation if the param
union gets unwieldy — decide when building Phase 5.)

Every instrument shares:
- the **MD toolkit** macro layer (§5),
- the **MD output stage** (§7) — AM → EQ → FLTF/FLTW/FLTQ filter → SRR → DIST,
- amp envelope params (`level`, `dec` per engine; velocity→accent §5.6),
- per-hit tolerance jitter (§5.5).

---## 5. Shared MD toolkit (new macros)

Add to `INSTRUMENT_PREAMBLE` in `lisp_host.rs` (they cost nothing unless used),
or inline per-instrument if we'd rather not touch the global preamble (decide
at Phase 1; preamble preferred — six instruments will use all of them).

### 5.1 `md_env` — retrigger-exact exponential decay
The workhorse. No de-click fade, no linear attack:
```lisp
(defmacro md_env (trig decay_ms)
  (make-history e)
  (def coef (exp (/ -6.907755 (max 1.0 (* decay_ms 0.001 samplerate)))))
  (def next (gswitch trig 1.0 (* (read-history e) coef)))
  (write-history e next)
  next)
```
Variants: `md_env_hold` (flat hold for `hold_ms` then exp decay — TRX-B2 HOLD),
`md_env_ar` (one-pole attack into exp decay — PI mallet, TRX-MA ATT/SUS).
Retrigger jumps straight to 1.0 — the GND-SN "pop" is authentic; the output
stage's final declick is the track fade, not ours.

### 5.2 `md_sweep` — exponential pitch ramp
`f(t) = base · 2^(ramp_semis · env / 12)` with `env = md_env(trig, rdec_ms)`.
Two sweeps per voice where called for: fast "punch" (1–15 ms) + slow "sigh"
(florid 808 detail: a few cents drifting down over ~300 ms).

### 5.3 `md_ping` — impulse-excited 2-pole resonator (bridged-T stand-in)
```
y[n] = 2 r cos(2π f/fs) y[n-1] − r² y[n-2] + g·x[n],   T60 ≈ -3 / (fs·log10(r))
```
Built from two scalar histories. Feed it `click`/shaped impulses → it rings and
decays by itself (the 808 way: "once excited, decays to silence"). Used for
PI modal banks, TRX-CL claves, TRX-RS, cowbell body.

### 5.4 `md_metal6` — the 808 metal cluster
Six `polyblep_pulse` (duty 0.48) oscillators at the measured 808 Schmitt-bank
frequencies **205.3, 304.4, 369.6, 522.7, 540, 800 Hz**, summed. Inharmonic by
design; exact values non-critical. `spread` param scales the cluster apart
(TRX GAP / MTAL territory), `tune` shifts it. Per-hit ±2 % jitter via latched
noise (tolerance modeling). Shared by hat + cymbal + cowbell engines (the 808
literally shares this bank across CB/CY/OH/CH).

### 5.5 `md_jitter` — per-hit component tolerance
`(def j1 (latch (noise) trigger))` etc.; map to ±2…5 % on oscillator
frequencies, ±10 % on decay times, scaled by a single `humanize` param
(0 = dead machine, default ~0.3). This alone kills the machine-gun effect.

### 5.6 Accent law
808 accent raises the trigger pulse voltage (4–14 V) — louder *and* harder.
Map velocity → output level **and** transient content: `accent = vel^1.5`;
transient/noise components scale by `accent`, body by `0.5 + 0.5·accent`,
sweep depth by `0.7 + 0.3·accent`.

### 5.7 `md_srr` + `md_bits` — sample-rate reduction & bit crush
SRR: sample-and-hold via `latch` clocked by a resettable phasor at
`f_sr = 44100 → 300 Hz` mapped exponentially. Bits: quantize
`floor(x·2^b)/2^b`, b = 12 → 2 (TRX-B2 DIRT goes "down as far as 2-bit").

### 5.8 `md_clip` / `md_harm` — waveshapers
- `md_clip`: hard-ish soft clip `clip(x·(1+9d), -1, 1)` crossfaded with
  `tanh(x·(1+3d))` — the MD CLIP/DIST flavor (edgier than plain tanh).
- `md_harm`: harmonics-adder for TRX-BD HARM: `sin(x·(1 + 4h))` phase
  overdrive on the *body oscillator output only* (pre-mix — principle #4).
- swing-VCA (cymbal): asymmetric clip `min(x, env)` style upper-edge clamp
  with soft knee exponent α ≈ 3.5 (Werner's fitted value) — clipping between
  envelope and signal is *the* 808 cymbal texture.

### 5.9 `md_fm_pair` — EFM core
Carrier sine @ `ptch` (+ sweep), modulator sine @ `mfrq` **in absolute Hz**,
modulator self-feedback `mfb` via one-sample history
(`m = sin(ph_m·2π + mfb·7·m_prev + …)`), index envelope
`idx = mod·13·md_env(trig, mdec)` rad into the carrier phase. The
feedback knob must reach the chaotic regime (community: "MFB 66 sounds
completely different than 65") — do **not** safety-limit it below chaos.

---

## 6. Engine specs (per instrument)

Parameter ranges in MD 0–127 spirit but expressed in real units. All engines
end in the shared output stage (§7). `ptch` tracks the host pitch input ±
`tune` semitones (MD pitch map: ~5.3 PTCH units/semitone — used for preset
conversion only).

### 6.1 `md-kick`

**TRX-BD** — the 808-style kick, modeled on Werner DAFx-14:
- Body: sine at `f0` 18–200 Hz with two-stage pitch env: STRT = first-6 ms
  boost of >1 octave (the 808's attack-frequency-shift "punch" — *not* a click
  sample), then RAMP/RDEC exponential sweep, plus a fixed subtle "sigh"
  (−1.5 semis over 300 ms).
- DEC 50–2500 ms exponential.
- NOIS: white noise → BP 900 Hz → 8 ms env, mixed pre-clip.
- HARM: `md_harm` on the body. CLIP: `md_clip` on the voice sum.
- Params: `ptch dec ramp rdec strt nois harm clip` (8, exactly the MD's).

**TRX-B2** — the techno/bassline kick:
- Same body; HOLD = flat amp hold 0–500 ms before decay (wobble-bass duty),
  TICK = filtered click transient (HP 4 kHz impulse), DIRT = `md_bits`
  (12→2 bits), DIST = `md_clip`. No RDEC (fixed fast sweep), per the manual.
- Params: `ptch dec ramp hold tick nois dirt dist`.

**EFM-BD**: `md_fm_pair` with carrier sweep RAMP/RDEC; MOD/MFRQ/MDEC/MFB.
MFRQ 10–2000 Hz log. Punch = short MDEC; grit = low MFB; chaos = high MFB.
- Params: `ptch dec ramp rdec mod mfrq mdec mfb`.

**PI-BD** — physically informed:
- 3-mode modal bank (`md_ping`s) at ratios 1.0 / 1.50 / 2.20 (membrane-ish,
  *not* harmonic), mode gains weighted by HARD; excitation = raised-cosine
  impulse whose width HAMR maps 0.3–8 ms (soft mallet = wide = dark).
- TENS: tension pitch glide — all mode freqs `f_k·(1 + β·E(t))` where `E` is
  the decaying hit energy (cheap: reuse mode-1 env²) and β = TENS·0.4·accent.
  This is the Avanzini/Marogna recipe and *the* acoustic-tom/kick signature.
- DAMP: scales all mode T60s down + raises damping of upper modes faster.
- Params: `ptch dec hard hamr tens damp` (6, like the MD).

**GND-SN**: pure sine + RAMP/RDEC sweep + DEC. The sub-layer tool. 4 params.

### 6.2 `md-snare`

**TRX-SD** — 909-style two-oscillator snare:
- Two sines at 180 Hz and 330 Hz (the drum's 0,1 modes; TUNE detunes the pair
  ratio, PTCH shifts both). Fast decays (these modes die 2× faster than the
  noise).
- BUMP/BENV: pitch shift at start with its own envelope (the 909 glissando).
- SNAP: noise split high/low — HP 2 kHz band with fast env + broadband with
  slower env, crossfaded by TONE; SNAP sets noise level vs body.
- CLIP on the sum.
- Params: `ptch dec bump benv snap tone tune clip`.

**EFM-SD**: `md_fm_pair` body (slightly inharmonic MFRQ ≈ 1.6×f0 default) +
noise layer with own NDEC + HPF. Params: `ptch dec noise ndec mod mfrq mdec hpf`.

**PI-SD** — the interesting one:
- Modal membrane (modes 1.0/1.59/2.14/2.65, `md_ping` bank) + TENS glide as
  PI-BD.
- **Wires (RVOL/RDEC): stochastic, not periodic.** PhISEM-style: rattle
  events with probability proportional to membrane energy `E(t)` (wires bounce
  while the head moves), each event injecting into noise → BP 3.5 kHz →
  `md_ping` 5 kHz. Decays with RDEC after the membrane stills.
- RING: one detuned metallic mode pair (ratios 6.27/6.5, beating) — shell ring.
- Params: `ptch dec hard tens rvol rdec ring`.

**RIM engines** (selector within md-snare, cheap): TRX-RS = two `md_ping`s
(480 Hz + 1.8 kHz, T60 ~80 ms) + DIST; EFM-RS = main FM hit + secondary FM
voice (SNAR/SPTC/SDEC/SMOD); PI-RS = ping pair + RVOL/RDEC rattle + RING.

### 6.3 `md-hat`

**TRX-HH** — 808 architecture, faithfully:
- `md_metal6` → BP (svf mode 1) at 3 440 Hz **and** 7 100 Hz (two parallel
  bands) → per-band exp VCA → HP 6–10 kHz.
- One engine covers CH and OH: `dec` 30–600 ms is the CH(~50 ms)↔OH(90–600 ms)
  continuum. GAP = cluster `spread` + band balance; MTAL = ring-mod the two
  trimmable oscillators (540/800 Hz pair) into each other + raise duty
  asymmetry; HPF/LPF = the machine-local filters.
- Params: `gap dec hpf lpf mtal` (5, like the MD) + `tune`.
- A `choke` UI affordance is the sequencer's concern, not the DSP's.

**EFM-HH**: `md_fm_pair` at high PTCH with FB pushed into the noisy regime +
TREM/TFRQ amplitude tremolo (the sizzle pulse — TFRQ up to ~300 Hz so it reads
as roughness, not wobble). Params: `ptch dec trem tfrq mod mfrq mdec fb`.

**PI-HH**:
- Inharmonic mode cluster (6 `md_ping`s, cymbal-like ratios 1/1.34/1.72/2.18/
  2.63/3.10 × PTCH·~400 Hz) + noise bed.
- CLSN: stochastic re-excitation — Poisson micro-collisions (two cymbals
  touching) whose rate scales with CLSN and current energy.
- CLOS: scheduled choke — at `t > clos_ms` multiply all mode decay coefs
  toward fast damping (127 = never closes). RING = mode-Q master.
- AG/AU/BR: high-shelf boost / high-shelf cut / low-shelf level (the manual's
  silver/gold/bronze spectral controls).
- Params: `ptch dec clsn ring ag au br clos`.

### 6.4 `md-cymbal`

**TRX-CY** — Werner's 808 cymbal topology, simplified to 2 bands:
- `md_metal6` → two BPs; band 1 (body ~3.4 kHz) with main DEC env; band 2
  (TOP, BP at TTUN 5–11 kHz) with its own faster env — TOP = its level.
- Swing-VCA soft clip (α≈3.5) per band (the texture!), then 3rd-order HP
  ~10.5 kHz on band 2 only, +6 dB/oct differentiator tilt on the sum.
- RICH = add the 2 trimmable oscillators detuned by latched jitter; SIZE =
  shift the whole cluster ÷1.0–1.6; PEAK = resonance of the band-2 BP.
- Params: `rich dec top ttun size peak`.

**EFM-CY**: as EFM-HH minus tremolo, plus HPF. **PI-RC/PI-CC**: PI-HH
architecture with bigger mode sets (8 pings), GRAB instead of CLOS, HARD =
excitation brightness; RC gets tighter mode spacing + longer T60, CC wider +
crashier (denser noise bed, faster initial bloom).

### 6.5 `md-tom`

**TRX-XT/XC** (same params, congas = higher tuning + harder default DTYP):
sine body + RAMP/RDEC sweep + DAMP (decay scale) + DIST/DTYP (`md_clip` hardness
morph). 909-tom style: add a touch of filtered noise at the attack.
**EFM-XT**: EFM-BD topology + CLIC instead of feedback. **PI-XT**: PI-BD + TUNE
(skin detune: split each mode into a beating pair), SIZE (mode freq scale ÷
body size), POS (sinusoidal mode weighting — center = fundamental, edge =
upper modes; STK ModalBar trick).

### 6.6 `md-perc`

**TRX-CP** — the 808 clap circuit, exactly:
- White noise → BP ~1 kHz (Q from TONE).
- Burst VCA: 3 sawtooth envelopes of ~10 ms (CLPY = count 2–5, RATE = spacing
  6–18 ms) then an uninterrupted ~20 ms decay. Implement with a resettable
  phasor at 1/RATE + a burst counter (`accum` reset on trigger, stop after
  CLPY wraps) driving `md_env` retriggers.
- Parallel "room" VCA: same filtered noise × single ~100 ms exp env; ROOM =
  level, RSIZ = decay 40–400 ms, RTUN = the tail's own BP center.
- HARD = clip drive; RICH = widen noise BP + add 2nd band.
- Params: `clpy tone hard rich rate room rsiz rtun` (8, the MD's own).

**TRX-CB**: two pulses 540 + 800 Hz (ratio 1.48) → BP 2.64 kHz with resonance
→ fast-strike + abrupt-stop envelope; ENH = body BP level, DAMP, TONE, BUMP =
start pitch shift. **TRX-CL**: single `md_ping` ~2.5 kHz, DUAL = double-strike
8 ms apart, CLIC = HP impulse. **TRX-MA**: noise → HP 5 kHz with ATT/SUS env,
RATL/RTYP = PhISEM grain layer mixed in, REV = reverse envelope shape.

**PI-MA** — pure PhISEM (Cook/STK Shakers calibration):
- Per sample: `shakeEnergy *= 0.999^(k_dec)`; trigger adds energy.
- Collision: if `uniform() < p(GRNS)` → `soundLevel += shakeEnergy·gain`,
  `gain ∝ log(N)/N`.
- `soundLevel *= 0.95^(k_glen)`; `out = soundLevel · noise → 2-pole res
  3 200 Hz, r = 0.96` (SIZE moves 2 000–5 500 Hz; HARD raises r + adds a
  second resonance ~5.6 kHz).
- Params: `dec grns glen size hard` (5, the MD's own; no pitch — authentic).

**PI-ML**: 4-mode metallic bank, Agogo-style ratios 1.0/4.08/6.67/9.0 with
near-coincident detune pairs for beating (the STK "beats" trick); TENS glide;
HARD = excitation width. Params: `ptch dec hard tens`.

---

## 7. The MD output stage (shared, every instrument)

In MD order, post-engine:
1. **AM** `amd` depth / `amf` rate 0.1 Hz–2 kHz (audio-rate AM = instant
   ring-mod dirt — keep the top of the range!).
2. **EQ**: one svf peak band, `eqf` 40 Hz–12 kHz, `eqg` ±15 dB.
3. **Filter**: FLTF/FLTW/FLTQ — HP edge at `fltf`, LP edge at
   `fltf + fltw` (both as note-spaced exponential frequency), each edge a
   2×svf cascade (24 dB) with Q `fltq` at both corners. `fltw` max = filter
   open (pure HP behavior), `fltf` min + `fltw` sweep = classic LP.
4. **SRR**: `md_srr` 0 → 300 Hz hold rate.
5. **DIST**: `md_clip` drive.
6. `level`, velocity accent law, final 1 ms anti-click ramp only on voice
   steal (the host gate fade, not inside the envelopes).

Optional global `lofi` toggle: 12-bit quantize + gentle 26 Hz–15 kHz band
limit, the UW converter character.

These ~10 params + engine params + amp keep each instrument well under the
operator instrument's param count. All continuous params `@mod true` where the
MD allows LFO targets (it allows everything — be generous).

---

## 8. UI plan

Operator-style mode-driven layout (proven in `core/operator` round 3):
- Left column: engine tabs (`ui-lego-mode-tab-s`) — e.g. TRX / B2 / EFM / PI /
  GND for md-kick — dispatching the center panel on
  `custom-ui-selected-section`.
- Center: the selected engine's 5–8 MD-named knobs (`ui-lego-*` faders/knobs),
  MD-style short uppercase labels (PTCH, RAMP, RDEC…).
- Right column: output stage (AM / EQ / FLT / SRR / DIST) + level — constant
  across engines, MD "track effects page" feel.
- Palette: dark chassis + per-family accent (TRX orange, EFM blue, PI green,
  GND grey) via `ui-lego-panel-x-s` accent stripes.
- Layout tests in `state_values.rs` per instrument (remember: only the default
  section's controls are assertable; register params in `test_param_map`).
- Use `each` for any generated control rows, never `map` (memory:
  lisp-ui-each-vs-map).

---

## 9. Verification plan (per engine, C harness)

Compile preamble+dsp per the harness method (`sed` preamble out of
lisp_host.rs, `swift run DGenLisp … --max-frames 128`, drive the dylib via
ctypes with the 6-arg `process`). Measurements, not vibes:

- **Kick**: instantaneous-pitch track (zero-crossing / STFT ridge) must show
  the exponential sweep landing on `ptch` with RDEC time constant; STRT
  visible as >1-octave boost confined to ≤8 ms; amp envelope linear-in-dB
  (exponential); DIRT/CLIP add measured THD.
- **Snare**: 180/330 Hz peaks decaying ≥2× faster than the noise band;
  PI-SD rattle spectrogram must be aperiodic (no comb lines — the 3d-snareo
  failure mode); TENS = measurable downward glide scaling with velocity.
- **Hat/cymbal**: spectral centroid high and *static* per hit (clusters must
  not track pitch unless tuned); CH→OH dec continuum; PI CLOS/GRAB shows a
  decay-rate knee at the scheduled time; EFM FB sweep: clean → sidebands →
  broadband chaos with no NaNs/denormals at FB max (clip the feedback path).
- **Clap**: envelope shows CLPY+1 humps at RATE spacing then a smooth tail.
- **Shaker**: collision counts/sec scale with GRNS; spectrum peak follows SIZE.
- **All**: two consecutive identical triggers differ (humanize > 0) but RMS
  within ±1 dB; velocity 0.3 vs 1.0 changes spectral centroid, not just level;
  no denormal CPU blowup on long tails (flush histories below 1e-20).
- Audition renders: 4-bar pattern per preset at 44.1k/48k, listen + compare
  against 808/909/MD reference hits (user has ears on this).

---

## 10. Presets

Per instrument, a `.presets` bank (JSON next to folder, drift format —
strip `__mod__*` manifest entries). Each bank: the classic targets first,
then abuse:
- md-kick: "808 Long", "808 Knock", "909 Punch", "B2 Wobble Sub", "EFM Click
  House", "PI Floor Tom Kick", "GND Sub Layer", "Gabber" (CLIP+DIRT maxed).
- md-snare: "808 Snappy", "909 Tight", "PI Brushy", "EFM Zap", "Rim Click",
  "Garage Skip".
- md-hat: "808 CH", "808 OH", "606 Crisp", "EFM Sizzle", "PI Loose Pair".
- md-cymbal: "808 Cymbal", "Ride Ping", "PI Crash", "EFM Trash".
- md-tom: "808 Tom Hi/Mid/Lo", "Conga", "PI Rototom", "EFM Lazer Tom".
- md-perc: "808 Clap", "Tight Clap", "808 Cowbell", "Claves", "PI Shaker",
  "PI Agogo", "Tambourine-ish".
Preset pitches via the MD pitch map (~5.3 units/semitone) where we're copying
known MD patches.

---

## 11. Build phases

Each phase = working instrument + harness verification + layout test + presets
before moving on. md-kick first because it exercises the whole toolkit.

1. **Toolkit + md-kick** (TRX-BD, TRX-B2, EFM-BD, PI-BD, GND-SN). Add §5
   macros to the preamble; prove md_env transient sharpness, sweep shapes,
   fm feedback chaos, modal TENS glide, jitter/accent laws. This phase
   de-risks everything else.
2. **md-snare** (+ rim engines). New ground: stochastic wire layer, BUMP/BENV.
3. **md-hat**. New ground: `md_metal6`, dual-band VCA, CLSN/CLOS scheduling.
4. **md-cymbal**. Swing-VCA texture, differentiator tilt, GRAB.
5. **md-tom + md-perc**. Clap burst machinery, PhISEM shaker, claves/cowbell.
6. **Sweep pass**: shared preset kit ("MD Kit 1" across all six), CPU audit
   (target: full 6-track kit ≪ one operator voice), delete `3d-drum`,
   `3d-snareo`, `drums/machinedrum-physical` after the user signs off A/B.

Open questions to resolve during Phase 1 (not blockers):
- Preamble vs per-file macros (preamble preferred).
- Whether engine switching should crossfade ~5 ms on change while playing.
- Whether md-perc splits into md-clap + md-perc.

---

## 12. Sources (research notes)

- Elektron Machinedrum User's Manual OS 1.63 rev M — verbatim Appendix A
  machine/param reference (all param tables in §6 use its wording).
  PDF: http://www.elektron.co.jp/wp-content/uploads/2015/01/machinedrum_manual_OS1.63.pdf
  (local extract was at /tmp/mdmanual/md163.txt — ephemeral; re-fetch if needed).
- Werner, Abel & Smith, *A Physically-Informed Circuit-Bendable Digital Model
  of the Roland TR-808 Bass Drum* (DAFx-14) — bridged-T 49.5/56 Hz, 1 ms
  trigger, 6 ms attack frequency shift >1 octave, pitch sigh via diode leakage,
  retrigger handling.
- Werner, Abel & Smith, *…TR-808 Cymbal* (ICMC/SMC 2014) — 6 Schmitt oscillators
  205.3/304.4/369.6/522.7 + 540/800 Hz trimmable, duty 47.98 %, swing-VCA
  α≈3.5, HP bands ~3 440/7 100 Hz, 3rd-order HP ~10.5 kHz, differentiator out.
- Sound on Sound Synth Secrets: Practical Bass Drum / Snare (180+330 Hz, 909
  noise split) / Cymbal / Cowbells & Claves (540+800 Hz → BP 2.64 kHz).
- Baratatronix Cascadia 808 circuit notes — clap (3×10 ms saw bursts + 100 ms
  tail, BP 1 kHz), hat decay constants (CH ~50 ms, OH 90–600 ms, CY 350–1200 ms).
- Perry Cook, *PhISM/PhISEM* (ICMC 1996) + STK `Shakers.cpp` (maraca:
  systemDecay 0.999, soundDecay 0.95, res 3 200 Hz r 0.96; cabasa/sekere/
  tambourine calibrations) and `ModalBar.cpp` (stick hardness rate
  `0.25·4^h`, position-weighted mode gains, marimba/agogo/"beats" ratios).
- Avanzini & Marogna / Pakarinen et al. — tension modulation: mode freqs
  ∝ (1 + β·E(t)), negligible extra cost.
- Elektronauts: MD voices diagram thread (2×DSP56303 @100 MHz, ~130
  instr/sample/voice, TRX by David Möllerstedt, EFM by Erik Larsson); EFM-BD
  recreation thread (MFB cliff at 66); pitch chart (~5.3 PTCH/semitone,
  github.com/davidguerette/md_pitch_map); X-firmware threads (GND-SW/PU/SN-PRO
  are unofficial-only).
- Sonic Charge Microtonic user guide — the closest modern TRX-alike (pitch-mod
  modes decaying/sine/random; clap-mode retrig envelope; noise Q→quasi-sine).
