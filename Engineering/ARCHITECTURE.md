# Digidrum Architecture

## 1. System boundary

The architecture is intentionally conservative outside sound generation. Digidrum should remain a deep modification of the stock OS while the stock sequencer, storage model, controls and downstream audio path are useful. Do not replace major firmware subsystems without a measured reason. Never modify the bootloader.


Digidrum replaces the sample-generation stage of selected Digitakt tracks with synthesis voices while retaining as much of the stock downstream audio path as possible.

Recommended signal flow:

```text
sequencer / trig / p-locks
        |
        v
custom machine parameter decode
        |
        v
synthesis voice
        |
        v
48 kHz track buffer
        |
        v
stock Digitakt overdrive
        |
        v
stock filter / amp / sends / mixer
```

The current spike injects synthesized samples into each active track's 32-sample buffer after stock playback and before overdrive.

### Platform-correctness gate

Before the machine framework is considered stable, verify machine switching, all parameter storage, p-locks, project reload, sound recall where applicable, copy/paste, multi-track triggering, FILTER/AMP behavior and stock-machine regressions. These are prerequisites for scaling the machine library, not cleanup work for later.

## 2. Source layout

Recommended repository structure:

```text
digidrum/
  include/
    dd_fixed.h
    dd_tables.h
    dd_envelope.h
    dd_osc.h
    dd_noise.h
    dd_filter.h
    dd_resonator.h
    dd_rate.h
    dd_machine.h
  dsp/
    fixed.c
    tables.c
    envelope.c
    osc.c
    noise.c
    filter.c
    resonator.c
    rate.c
  machines/
    trx_bd.c
    trx_b2.c
    trx_sd.c
    trx_xt.c
    trx_hat.c
    efm_bd.c
    efm_sd.c
    efm_tom.c
    pi_bd.c
    metal_cluster.c
    digital_debris.c
  platform/
    digitakt_glue.c
    digitakt_glue.s
    machine_registry.c
  tests/
    test_fixed.c
    test_envelope.c
    test_resonator.c
    test_machine_*.c
    render_wav.c
```

Keep machine code independent from patch addresses and Elektron-specific glue wherever possible.

## 3. Layering

### DSP primitives

Pure C99 and deterministic. No firmware patch knowledge.

Examples:

- Q15 multiply / clamp / lerp
- phase accumulator
- sine table lookup
- exponential-decay accumulator
- one-pole LP/HP
- 2-pole resonator
- LFSR / xorshift noise
- waveshapers
- decimator / zero-order-hold / linear interpolation

### Machine layer

Each instrument owns only its synthesis state and semantic parameter mapping.

Examples:

- TRX-B2: swept body + hold stage + click/noise + digital dirt
- EFM-BD: carrier/modulator FM with decaying modulation index and feedback
- PI-BD: impulse into a small modal bank

### Platform layer

Responsible for:

- machine registration
- page / parameter routing
- p-lock transport
- render hook
- track state ownership
- firmware patching

Do not put synthesis math in assembly glue.

## 4. Voice lifecycle

Each track has one persistent voice state per active machine.

```text
init -> idle -> trigger -> active render blocks -> silence detection -> idle
```

A trigger should reset only the state that is musically expected to reset. Some future machines may deliberately preserve oscillator/random state between triggers.

## 5. Sample-rate policy

The host / output path remains at the Digitakt engine rate.

Each machine chooses an internal synthesis rate:

```text
FULL   = host rate
HALF   = host / 2
QUARTER= host / 4
```

Recommended defaults:

| Machine class | Suggested internal rate |
| --- | --- |
| bass drum / tom resonators | HALF |
| FM drums | HALF |
| modal / physical models | HALF unless unstable |
| metallic clusters | HALF or QUARTER |
| noisy / digital / byte-style percussion | QUARTER |
| final track buffer | FULL |

The purpose is CPU reduction first. Aliasing can be accepted or exploited for character where appropriate.

## 6. Parameter convention

Use seven synthesis parameters plus level when mapping to the eight SRC controls.

Suggested UI contract:

```text
A  primary pitch / root
B  character / mode / ratio
C  tone / structure
D  transient / punch / secondary macro
E  sweep / modulation
F  decay
G  drive / dirt / feedback
H  level
```

Individual machines may rename these, but parameter order should remain broadly consistent when possible.

## 7. No global 12-bit engine

Do not reduce the entire engine to 12-bit merely to emulate older digital drum machines.

Instead:

- keep accumulators and the mixer at sufficient precision;
- quantize selected internal nodes where useful;
- run suitable voices at lower sample rates;
- reduce control-rate work;
- use lookup tables and recursive state updates.

Bit depth is primarily a timbral choice. Sample-rate reduction and cheaper arithmetic are more relevant to CPU savings.

## 8. Machine registry

Each machine descriptor should eventually contain:

```c
struct dd_machine_desc {
    uint8_t id;
    const char *name;
    uint8_t internal_rate_div;
    uint16_t state_size;
    void (*init)(void *voice);
    void (*render)(void *voice,
                   const struct dd_params *p,
                   int trigger,
                   int32_t *out,
                   uint32_t size);
};
```

The first implementation can keep the existing hard-wired ABI, but the end state should make adding a machine mostly declarative.
