/* SPDX-License-Identifier: MIT */
#ifndef DD_RATE_H
#define DD_RATE_H

#include <stdint.h>
#include "dd_fixed.h"

typedef struct {
    int32_t prev;
    int32_t next;
    uint8_t have_next;
} dd_zoh;

static inline void dd_zoh_init(dd_zoh *z, int32_t initial)
{
    z->prev = initial;
    z->next = initial;
    z->have_next = 0;
}

static inline int32_t dd_zoh_step(dd_zoh *z, int32_t input)
{
    z->prev = z->next;
    z->next = input;
    z->have_next = 1;
    return z->prev;
}

typedef struct {
    int32_t prev;
    int32_t next;
    int32_t phase;
    int32_t step;
} dd_interp;

static inline void dd_interp_init(dd_interp *i, int32_t initial, int32_t step)
{
    i->prev = initial;
    i->next = initial;
    i->phase = 0;
    i->step = step;
}

static inline int32_t dd_interp_step(dd_interp *i, int32_t input)
{
    int32_t out;
    i->prev = i->next;
    i->next = input;
    i->phase += i->step;
    if (i->phase >= 32768) {
        i->phase -= 32768;
        out = i->prev;
    } else {
        out = dd_lerp_q15(i->prev, i->next, i->phase);
    }
    return out;
}

typedef struct {
    uint8_t tick;
} dd_downsampler;

static inline void dd_downsampler_init(dd_downsampler *d)
{
    d->tick = 0;
}

static inline int dd_downsampler_should_process(dd_downsampler *d)
{
    ++d->tick;
    if (d->tick >= 2) {
        d->tick = 0;
        return 1;
    }
    return 0;
}

#endif
