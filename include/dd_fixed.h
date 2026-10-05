/* SPDX-License-Identifier: MIT */
#ifndef DD_FIXED_H
#define DD_FIXED_H

#include <stdint.h>

#define DD_Q15_ONE      32767
#define DD_Q15_HALF     16384
#define DD_Q15_SCALE    32768

static inline int32_t dd_mul_q15(int32_t a, int32_t b)
{
    return (a * b) >> 15;
}

static inline int32_t dd_abs32(int32_t x)
{
    return x < 0 ? -x : x;
}

static inline int32_t dd_clamp(int32_t x, int32_t lo, int32_t hi)
{
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

static inline int32_t dd_clamp_q15(int32_t x)
{
    if (x > DD_Q15_ONE)  return DD_Q15_ONE;
    if (x < -DD_Q15_ONE) return -DD_Q15_ONE;
    return x;
}

static inline int16_t dd_clamp_s16(int32_t x)
{
    if (x > 32767)  return 32767;
    if (x < -32768) return -32768;
    return (int16_t)x;
}

static inline int32_t dd_sadd_q15(int32_t a, int32_t b)
{
    int32_t r = a + b;
    if (r > DD_Q15_ONE)  return DD_Q15_ONE;
    if (r < -DD_Q15_ONE) return -DD_Q15_ONE;
    return r;
}

static inline int32_t dd_ssub_q15(int32_t a, int32_t b)
{
    int32_t r = a - b;
    if (r > DD_Q15_ONE)  return DD_Q15_ONE;
    if (r < -DD_Q15_ONE) return -DD_Q15_ONE;
    return r;
}

static inline int32_t dd_lerp_q15(int32_t a, int32_t b, int32_t t)
{
    return a + dd_mul_q15(b - a, t);
}

static inline int32_t dd_soft_clip(int32_t x, int32_t drive)
{
    int32_t gain = DD_Q15_ONE + (drive >> 1);
    int32_t y = dd_mul_q15(x, gain);
    int32_t a = dd_abs32(y);

    if (a > 16384) {
        int32_t over = a - 16384;
        a = 16384 + over - dd_mul_q15(over, over);
        if (a > DD_Q15_ONE)
            a = DD_Q15_ONE;
        y = y < 0 ? -a : a;
    }
    return y;
}

static inline int32_t dd_hard_clip(int32_t x, int32_t level)
{
    if (x > level)  return level;
    if (x < -level) return -level;
    return x;
}

static inline int32_t dd_bit_quantize(int32_t x, int32_t bits)
{
    int32_t step_shift;
    if (bits >= 16)
        return dd_clamp_q15(x);
    if (bits < 1)
        bits = 1;
    step_shift = 16 - bits;
    return (x >> step_shift) << step_shift;
}

#endif
