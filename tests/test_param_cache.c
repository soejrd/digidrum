/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "dd_param_cache.h"

int main(void)
{
    dd_param_cache cache;
    struct dd_params p = {{0}, 0};
    dd_param_cache_init(&cache);

    assert(dd_param_cache_update(&cache, &p) == DD_ALL_PARAMS_CHANGED);
    assert(dd_param_cache_update(&cache, &p) == 0);

    p.p[DD_PITCH] = 1234;
    p.p[DD_DECAY] = 5678;
    p.level = 9000;
    assert(dd_param_cache_update(&cache, &p) ==
           (DD_PARAM_CHANGED(DD_PITCH) | DD_PARAM_CHANGED(DD_DECAY) | DD_LEVEL_CHANGED));
    assert(cache.raw.p[DD_PITCH] == 1234);
    assert(cache.raw.p[DD_DECAY] == 5678);
    assert(cache.raw.level == 9000);
    assert(dd_param_cache_update(&cache, &p) == 0);

    dd_param_cache_init(&cache);
    assert(dd_param_cache_update(&cache, &p) == DD_ALL_PARAMS_CHANGED);
    printf("ok: parameter cache\n");
    return 0;
}
