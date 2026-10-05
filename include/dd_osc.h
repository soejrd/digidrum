/* SPDX-License-Identifier: MIT */
#ifndef DD_OSC_H
#define DD_OSC_H

#include <stdint.h>

typedef struct {
    uint32_t phase;
} dd_osc;

static inline void dd_osc_init(dd_osc *o)
{
    o->phase = 0;
}

static inline void dd_osc_set_phase(dd_osc *o, uint32_t phase)
{
    o->phase = phase;
}

int32_t dd_osc_sine(dd_osc *o, uint32_t inc);
int32_t dd_osc_sine_interp(dd_osc *o, uint32_t inc);
int32_t dd_osc_square(dd_osc *o, uint32_t inc, uint32_t width);
int32_t dd_osc_triangle(dd_osc *o, uint32_t inc);
int32_t dd_osc_saw(dd_osc *o, uint32_t inc);

#endif
