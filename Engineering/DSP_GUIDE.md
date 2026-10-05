# Digidrum DSP Guide

## 1. Hard constraints

- C99.
- ColdFire 54455 target.
- `m68k-elf-gcc`.
- No assumptions about cheap 64-bit arithmetic.
- Avoid per-sample `sin`, `pow`, `exp`, `tanh`, division and modulus unless benchmarked.
- Existing code uses Q15-style fixed-point arithmetic and 32-bit state.
- Audio is rendered in 32-sample blocks.

## 2. Fixed-point rules

Use explicit formats in names or comments.

Recommended conventions:

```text
Q15      control values / normalized gains
Q1.31    higher-precision audio or filter state where safe
uint32   phase accumulators
Q16.16   optional frequency / time values outside normalized range
```

Core helper:

```c
static inline int32_t dd_mul_q15(int32_t a, int32_t b)
{
    return (a * b) >> 15;
}
```

Be careful: `a * b` must remain inside signed 32-bit range for the chosen operand bounds.

## 3. Required primitive library

Implement these before building many machines.

### Fixed math

- `dd_mul_q15`
- `dd_clamp_q15`
- `dd_abs32`
- `dd_lerp_q15`
- saturating add/sub
- cheap soft clip
- hard clip

### Oscillators

- 32-bit phase accumulator
- sine lookup table
- triangle
- square / pulse
- optionally cheap phase distortion

### Envelopes

Prefer recursive envelopes:

```c
env = dd_mul_q15(env, coeff);
```

Do not recompute exponentials per sample.

Needed types:

- one-stage decay
- attack/hold/decay
- pitch sweep decay
- two-stage transient + body

### Noise

One shared cheap RNG implementation:

- xorshift32 or equivalent
- bipolar Q15 output
- optional clocked/sample-and-hold noise

A shared RNG implementation does not require every voice to share the same RNG *state* if decorrelation is needed.

### Filters / resonators

Minimum set:

- one-pole LP
- one-pole HP
- 2-pole state-variable or equivalent
- damped 2-pole resonator

A large fraction of drum synthesis can be built from impulse/noise excitation plus cheap resonators.

### Nonlinear blocks

- hard clip
- cubic / polynomial soft clip
- fold
- bit quantizer
- sample-rate / hold reduction

## 4. Multi-rate rendering

Do not render every subsystem at the host rate by default.

Example policy:

```text
oscillator/resonator   24 kHz equivalent
amp envelope            6-12 kHz update
pitch envelope          6-12 kHz update
parameter mapping       1-3 kHz update
output buffer           48 kHz equivalent
```

For a half-rate voice:

```text
compute x0
emit x0, interpolated(x0,x1)
compute x1
...
```

Three reconstruction modes are worth supporting:

1. zero-order hold — cheapest, can sound intentionally digital;
2. linear interpolation — still cheap, less imaging;
3. optional small low-pass — only for machines that require it.

## 5. Control-rate caching

Never remap 0..127 / Q15 controls into expensive coefficients on every sample.

At block start or on parameter change, precompute:

- oscillator increments
- decay coefficients
- filter coefficients
- resonance coefficients
- gain scalars
- waveshaper thresholds

If a parameter lock changes per trig, recompute once on trigger / block boundary.

Use `dd_param_cache` once at the start of each render call. Its change mask
identifies which raw controls changed since the previous block; each machine
maps only those controls into its own derived coefficients. Call it before
handling a trigger so parameter locks apply to that trigger. Keep dynamic
envelope and oscillator state separate from the cached control values.

## 6. Lookup tables

Recommended tables:

- sine: 256-1024 entries
- exponential decay coefficient curves
- pitch / frequency mapping
- optional waveshaper transfer curve

Use power-of-two table sizes where it simplifies phase indexing.

## 7. Intentional digital coloration

Apply quantization locally, not globally.

Useful locations:

```text
FM feedback state
metallic oscillator sum
noise path
post-body/pre-distortion node
sample-and-hold stage
```

Possible effective resolutions:

```text
18-20 bit: subtle old-digital texture
14-16 bit: obvious grit
10-12 bit: strong quantization
6-8 bit: effect territory
```

These are timbral targets, not claims about original machine internals.

## 8. Machine-specific CPU budgets

Every machine should document an approximate cost class:

```text
A  very cheap: <= a few oscillators / filters
B  cheap: FM pair / small resonator set
C  medium: 4-8 oscillator metallic cluster
D  expensive: larger modal bank / oversampled nonlinear block
```

Until hardware profiling exists, treat these as relative labels only.

## 9. Optimization order

Optimize in this order:

1. remove transcendentals from sample loops;
2. lower control update rate;
3. lower synthesis rate per machine;
4. cache parameter mappings;
5. simplify oscillator / filter topology;
6. switch arithmetic representation if benchmark data supports it;
7. only then micro-optimize C or assembly.

Do not spend effort making a mathematically elegant algorithm fast if a cheaper topology sounds equally good.
