# Instrument API and Coding Contract

## 1. Current required ABI

New instruments are written in C99 and expose the existing voice entry points:

```c
void dp_voice_init(struct dp_voice *v);

void dp_voice_render(
    struct dp_voice *v,
    const struct dp_params *p,
    int trigger,
    int32_t *out,
    uint32_t size);
```

The current render block is normally 32 samples.

## 2. Current parameter structure

```c
struct dp_params {
    uint16_t frequency;
    uint16_t tone;
    uint16_t decay;
    uint16_t attack_fm;
    uint16_t self_fm;
    uint16_t drive;
    uint16_t accent;
    uint16_t level;
};
```

This structure predates the multi-machine plan. Do not let historical field names constrain new synthesis semantics.

## 3. Proposed generic parameter layer

Introduce a neutral machine-facing representation:

```c
struct dd_params {
    uint16_t p[7];
    uint16_t level;
};
```

The portable TRX-BD/B2/SD prototypes use `dd_trx_params` instead: eight
machine controls plus a separate track level. The Digitakt adapter now maps
all eight SRC slots to these controls and leaves level to the stock track AMP
path. The patched image builds; emulator behavior still needs verification.

Optional helpers:

```c
#define DD_PITCH 0
#define DD_P2    1
#define DD_P3    2
#define DD_P4    3
#define DD_P5    4
#define DD_DECAY 5
#define DD_DRIVE 6
```

Each machine provides semantic aliases locally:

```c
#define B2_PITCH p[0]
#define B2_HOLD  p[1]
#define B2_TICK  p[2]
#define B2_NOISE p[3]
#define B2_SWEEP p[4]
#define B2_DECAY p[5]
#define B2_DIRT  p[6]
```

The UI layer remains free to display different names.

## 4. Voice state rules

Voice state should contain only persistent synthesis state.

Good:

- phase accumulators
- resonator history
- envelope levels
- RNG state
- interpolation history
- active flag / silence counter

Avoid storing:

- UI labels
- patch addresses
- redundant derived coefficients that can be recalculated once per block unless caching is beneficial

## 5. Machine implementation pattern

Recommended file shape:

```c
#include "dd_fixed.h"
#include "dd_osc.h"
#include "dd_envelope.h"
#include "dd_machine.h"

typedef struct {
    uint32_t phase;
    int32_t amp_env;
    int32_t pitch_env;
    uint16_t quiet_samples;
    uint8_t active;
} dd_trx_b2_voice;

void dd_trx_b2_init(dd_trx_b2_voice *v)
{
    /* zero/init state */
}

void dd_trx_b2_render(
    dd_trx_b2_voice *v,
    const struct dd_params *p,
    int trigger,
    int32_t *out,
    uint32_t size)
{
    /* map/cached params once */
    /* reset trigger state */
    /* render N samples */
    /* update silence detector */
}
```

## 6. Trigger semantics

On trigger:

1. apply accent / velocity-derived scaling;
2. initialize envelopes;
3. initialize transient generators;
4. reset phase only if the intended instrument requires deterministic phase;
5. optionally preserve random / resonator state for machines designed to vary per hit.

Document reset behavior in every machine source file.

## 7. Silence / active handling

Voices should stop doing expensive work once inaudible.

Recommended rule:

```
if abs(output/state energy) < threshold for N samples:
    active = 0
```

Do not use one sample crossing zero as a sleep criterion.

## 8. Machine header comment

Every machine source should begin with:

```
Machine:
Intent:
Reference family:
Not intended as exact clone of:
Internal sample-rate divisor:
CPU class:
Parameters A-G:
Trigger reset behavior:
DSP blocks used:
Known limitations:
License / provenance notes:
```

This is important for agent handoffs and future clean-room review.

## 9. Build gates

A new instrument is not done until it passes:

- host unit tests
- deterministic render test
- output bounds test
- cross-compile
- link check
- Digiemu smoke test
- p-lock test
- simultaneous track test
- hardware test when hardware is available

## 10. Language and Target Details

### Why C?

New instruments for digidrum should be written in **C (C99 standard)** with fixed-point DSP arithmetic.

- The digidrum project targets the **Motorola ColdFire 54455 CPU** (m68k architecture) on the original Digitakt Mk1
- Uses the cross-compiler `m68k-elf-gcc` with specific embedded flags
- Fixed-point Q15 arithmetic is required for real-time audio processing within the CPU's constraints
- The current TRX and EFM instruments are written in C

### Fixed-Point Arithmetic

- **Q15 format**: 16-bit signed values with implicit binary point
- Parameters range: 0..32767 (unsigned Q15)
- Audio values: ±32767 (clamped 16-bit)
- Multiplication macro: `(a * b) >> 15` (see the shared helpers in `include/dd_fixed.h`)
- No 64-bit helpers on the ColdFire target — all operations must fit in 32-bit

## 11. Integration and Related Projects

### Integration Steps

1. **Implement the voice functions** in a new `.c` file
2. **Define the header** with structs and function prototypes
3. **Register the machine** via elekloader patches (see `glue.s` and existing mods)
4. **Build and test** with `make cross-check` and `make test`
5. **Verify in Digiemu** emulator before hardware testing

### Related Projects

- `digiperc/` — earlier percussion synthesis experiments
- `digisophie/` — custom SRC machine using Digitakt's audio path
- `digineighbor/` — another drum instrument
- `schwung-sophie/` — VST3 plugin version of sophie DSP

### See Also

- `digidrum/machines/` — current instrument implementations
- `digidrum/include/` — shared machine interfaces and DSP helpers
- `digidrum/Makefile` — build configuration
- `digidrum/glue.s` — assembly glue code for integration
