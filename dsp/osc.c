/* SPDX-License-Identifier: MIT */
#include "../include/dd_osc.h"
#include "../include/dd_tables.h"
#include "../include/dd_fixed.h"

int32_t dd_osc_sine(dd_osc *o, uint32_t inc)
{
    o->phase += inc;
    return dd_sine_tab[o->phase >> (32u - DD_SINE_SIZE_LOG2)];
}

int32_t dd_osc_sine_interp(dd_osc *o, uint32_t inc)
{
    o->phase += inc;
    uint32_t idx = o->phase >> (32u - DD_SINE_SIZE_LOG2);
    uint32_t frac = o->phase & ((1u << (32u - DD_SINE_SIZE_LOG2)) - 1u);
    int32_t s0 = dd_sine_tab[idx & (DD_SINE_SIZE - 1)];
    int32_t s1 = dd_sine_tab[(idx + 1u) & (DD_SINE_SIZE - 1)];
    return dd_lerp_q15(s0, s1, (int32_t)(frac >> 8));
}

int32_t dd_osc_square(dd_osc *o, uint32_t inc, uint32_t width)
{
    return dd_osc_square_fast(o, inc, width);
}

int32_t dd_osc_triangle(dd_osc *o, uint32_t inc)
{
    o->phase += inc;
    uint32_t p = (o->phase >> 16) & 0xFFFFu;
    uint32_t t = (p < 32768u) ? p : (0xFFFFu - p);
    return (int32_t)(t - 16384) * 2;
}

int32_t dd_osc_saw(dd_osc *o, uint32_t inc)
{
    o->phase += inc;
    return dd_clamp(-32767 + (int32_t)(o->phase >> 16), -32767, 32767);
}
