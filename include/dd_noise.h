/* SPDX-License-Identifier: MIT */
#ifndef DD_NOISE_H
#define DD_NOISE_H

#include <stdint.h>

typedef struct {
    uint32_t state;
} dd_noise;

static inline void dd_noise_init(dd_noise *n, uint32_t seed)
{
    if (seed == 0)
        seed = 0xDEADBEEF;
    n->state = seed;
}

static inline void dd_noise_seed(dd_noise *n, uint32_t seed)
{
    dd_noise_init(n, seed);
}

static inline int32_t dd_noise_next(dd_noise *n)
{
    n->state ^= n->state << 13;
    n->state ^= n->state >> 17;
    n->state ^= n->state << 5;
    return (int32_t)(n->state >> 16) - (int32_t)(n->state & 0xFFFF);
}

static inline int32_t dd_noise_q15(dd_noise *n)
{
    n->state ^= n->state << 13;
    n->state ^= n->state >> 17;
    n->state ^= n->state << 5;
    return (int32_t)(n->state >> 16) - (int32_t)(n->state & 0xFFFF);
}

static inline int16_t dd_noise_s16(dd_noise *n)
{
    return (int16_t)(dd_noise_q15(n) >> 15);
}

#endif
