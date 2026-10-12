# EFM-BD

## 1. What the instrument is

EFM-BD is a **two-operator FM voice** with **three one-pole envelopes** and
**feedback**. In words, per output sample:

1. Operator 1 is a sine oscillator whose phase is *phase-modulated* by a
   running value `mod` (this is a classic self-feedback "metallic" trick).
2. Operator 1's output `s1` both (a) feeds the operator-2 *amplitude/FM* chain
   and (b) reloads `mod` for the next sample.
3. Operator 2 is the main (carrier) sine; its frequency and its amplitude are
   driven by operator 1.
4. A third, unsigned-in-time decay envelope `fb` scales the final output and
   *also* seeds the whole thing at trigger time (that is `RAMP`).

---

## 2. The one function that matters: `bd_sample()`

```c
const double op1_prev = s->op1;
s->op1 = c->rdec * op1_prev;                              // envelope 1 decay
const double y1 = c->ptch * (1.0 + 8.0 * op1_prev);       // op1 rate

const int    i1 = ((int)floor(s->ph1 + s->mod)) & 0x7fff; // mod -> phase index
const double s1 = sin(TWO_PI * (double)i1 / 32768.0);     // operator 1

s->ph1 += 262144.0 * c->mfrq * y1;                        // op1 phase advance
s->mod  = 32768.0 * c->mfb * s1;                          // op1 phase-mod reload

const double op2_prev = s->op2;
s->op2 = c->mdec * op2_prev;                              // envelope 2 decay
const double c0  = op2_prev * s1;                         // op1 -> op2
const double x1c = c0 * y1;                               // FM depth

const int    i2 = ((int)floor(s->ph2)) & 0x7fff;
const double s2 = sin(TWO_PI * (double)i2 / 32768.0);     // operator 2
s->ph2 += 32768.0 * (8.0 * c->mod * x1c + y1);            // op2 phase advance

const double out = s->fb * s2;                            // output
s->fb = c->dec * s->fb;                                   // feedback decay
return out;
```

Six state numbers: `ph1, ph2, mod, op1, op2, fb`.

---

## 3. Knobs -> coefficients (`bd_make_controls`)

Four controls are exactly linear in the knob `v` (0..127); four follow a clean
exponential. The exponential coefficients were fitted by sampling the
instrument's response across the full knob travel and regressing the decay
rate; the linear controls matched a straight line closely enough to keep as-is.

| control | coefficient | rule |
| --- | --- | --- |
| PTCH | `ptch` | `0.000581 * 2^(v/32)` |
| DEC  | `dec`  | `1 - 0.009469 * 2^(-v/13.5)` |
| RAMP | `ramp` | `(v - 16) / 128` |
| RDEC | `rdec` | `1 - 0.009468 * 2^(-v/12.6)` |
| MOD  | `mod`  | `v / 128` |
| MFRQ | `mfrq` | `max(1, v) / 128` |
| MDEC | `mdec` | `1 - 0.009474 * 2^(-v/11.52)` |
| MFB  | `mfb`  | `v / 128` |

Notes:
- `RAMP` is *signed*: knob 16 is 0, knob 0 is -0.125, knob 127 is +0.867. It
  seeds operator 1 at trigger and therefore sets the attack "knock".
- `DEC`, `RDEC`, `MDEC` are all close to 1.0 (slow decay) and get slightly
  slower as the knob rises; they are per-sample one-pole coefficients, so
  `dec = 0.9996` (knob 64) is roughly a 0.1 s time constant at 44.1 kHz.

---

## 4. Trigger and init

```c
void bd_init(bd_state *s)
{
    s->ph1 = s->ph2 = s->mod = 0.0;
    s->op1 = s->op2 = s->fb  = 0.0;
}

void bd_trigger(bd_state *s, const bd_controls *c)
{
    s->op1 = c->ramp;           // attack seed
    s->op2 = 1.0;               // full scale
    s->fb  = 0.630957;          // feedback seed level
    s->ph1 = s->ph2 = 4000.0;   // start phase, index units
    s->mod = 0.0;
}
```

---

## 5. Porting checklist

1. Implement `sin`, `floor`, and `pow` (or precompute `2^(v/32)` etc. for the
   eight knob values you care about).
2. Copy `bd_sample` verbatim; use `float` instead of `double` if you like
   (the values are well conditioned).
3. Run `bd_trigger` once, then call `bd_sample` per output frame at 44.1 kHz.
