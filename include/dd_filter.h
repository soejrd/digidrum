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

int32_t dd_onepole_lp_run(dd_onepole *f, int32_t input, int32_t coeff);

#endif
