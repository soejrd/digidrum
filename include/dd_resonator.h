/* SPDX-License-Identifier: MIT */
#ifndef DD_RESONATOR_H
#define DD_RESONATOR_H

#include <stdint.h>
#include "dd_fixed.h"

typedef struct {
    int32_t low;
    int32_t band;
    int32_t freq;
    int32_t damping;
} dd_resonator;

static inline void dd_resonator_init(dd_resonator *r)
{
    r->low = 0;
    r->band = 0;
    r->freq = 0;
    r->damping = 0;
}

static inline void dd_resonator_set(dd_resonator *r, int32_t freq, int32_t damping)
{
    r->freq = freq;
    r->damping = damping;
}

static inline int32_t dd_resonator_lp(dd_resonator *r, int32_t input)
{
    int32_t high = input - r->low - dd_mul_q15(r->band, r->damping);
    r->band = dd_clamp(dd_mul_q15(r->band, r->freq) + high, -65535, 65535);
    r->low = dd_mul_q15(r->band, r->freq) + r->low;
    return r->low;
}

static inline int32_t dd_resonator_bp(dd_resonator *r, int32_t input)
{
    int32_t high = input - r->low - dd_mul_q15(r->band, r->damping);
    r->band = dd_clamp(dd_mul_q15(r->band, r->freq) + high, -65535, 65535);
    r->low = dd_mul_q15(r->band, r->freq) + r->low;
    return r->band;
}

static inline int32_t dd_resonator_hp(dd_resonator *r, int32_t input)
{
    int32_t high = input - r->low - dd_mul_q15(r->band, r->damping);
    r->band = dd_clamp(dd_mul_q15(r->band, r->freq) + high, -65535, 65535);
    r->low = dd_mul_q15(r->band, r->freq) + r->low;
    return high;
}

#endif
