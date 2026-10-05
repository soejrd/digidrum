/* SPDX-License-Identifier: MIT */
#ifndef DD_PARAM_CACHE_H
#define DD_PARAM_CACHE_H

#include <stdint.h>
#include "dd_machine.h"

#define DD_PARAM_CHANGED(index) ((uint16_t)(1u << (index)))
#define DD_LEVEL_CHANGED DD_PARAM_CHANGED(DD_PARAMS_COUNT)
#define DD_ALL_PARAMS_CHANGED ((uint16_t)((1u << (DD_PARAMS_COUNT + 1)) - 1u))

/* One cache per voice. Call once at each render block, before any samples. */
typedef struct {
    struct dd_params raw;
    uint8_t valid;
} dd_param_cache;

static inline void dd_param_cache_init(dd_param_cache *cache)
{
    cache->valid = 0;
}

/* Returns bits for controls changed since the preceding block. */
static inline uint16_t dd_param_cache_update(dd_param_cache *cache,
                                              const struct dd_params *params)
{
    uint16_t changed = 0;
    uint32_t i;

    for (i = 0; i < DD_PARAMS_COUNT; ++i) {
        if (!cache->valid || cache->raw.p[i] != params->p[i]) {
            cache->raw.p[i] = params->p[i];
            changed |= DD_PARAM_CHANGED(i);
        }
    }
    if (!cache->valid || cache->raw.level != params->level) {
        cache->raw.level = params->level;
        changed |= DD_LEVEL_CHANGED;
    }
    cache->valid = 1;
    return changed;
}

#endif
