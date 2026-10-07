/* SPDX-License-Identifier: MIT */
#ifndef DD_FILTER_H
#define DD_FILTER_H

#include <stdint.h>
#include "dd_fixed.h"

typedef struct {
    int32_t lp;
    int32_t hp;
} dd_onepole;

static inline void dd_onepole_init(dd_onepole *f)
{
    f->lp = 0;
    f->hp = 0;
}

static inline int32_t dd_onepole_lp(dd_onepole *f, int32_t input, int32_t coeff)
{
    f->lp = dd_mul_q15(f->lp, coeff) + dd_mul_q15(input, DD_Q15_ONE - coeff);
    return f->lp;
}

static inline int32_t dd_onepole_hp(dd_onepole *f, int32_t input, int32_t coeff)
{
    f->lp = dd_mul_q15(f->lp, coeff) + dd_mul_q15(input, DD_Q15_ONE - coeff);
    f->hp = input - f->lp;
    return f->hp;
}

/* One multiply per stage for bounded Q15 input and state. Use when a small
 * rounding difference from dd_onepole_lp/hp is acceptable. */
static inline int32_t dd_onepole_lp_fast(dd_onepole *f, int32_t input,
                                          int32_t coeff)
{
    f->lp += dd_mul_q15(input - f->lp, DD_Q15_ONE - coeff);
    return f->lp;
}

static inline int32_t dd_onepole_hp_fast(dd_onepole *f, int32_t input,
                                          int32_t coeff)
{
    f->lp += dd_mul_q15(input - f->lp, DD_Q15_ONE - coeff);
    f->hp = input - f->lp;
    return f->hp;
}

/* Constant-peak bandpass for a parametric EQ boost. Coefficients are Q14;
 * input and state are Q15. The b1 term is zero, so one band needs three
 * multiplies per sample. Configure coefficients outside the audio loop. */
typedef struct {
    int32_t b0, a1, a2;
    int32_t x2, y1, y2;
} dd_eq_band;

static inline void dd_eq_band_init(dd_eq_band *f)
{
    f->x2 = f->y1 = f->y2 = 0;
}

static inline int32_t dd_eq_band_run(dd_eq_band *f, int32_t input)
{
    int32_t sum = f->b0 * (input - f->x2) -
                  f->a1 * f->y1 - f->a2 * f->y2;
    int32_t output = dd_clamp_q15(sum >> 14);
    f->x2 = input;
    f->y2 = f->y1;
    f->y1 = output;
    return output;
}

int32_t dd_onepole_lp_run(dd_onepole *f, int32_t input, int32_t coeff);

#endif
