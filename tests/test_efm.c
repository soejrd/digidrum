/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "dd_efm.h"

int main(void)
{
    uint32_t kind, block, i;
    for (kind = 0; kind < DD_EFM_MACHINE_COUNT; ++kind) {
        dd_efm_voice voice;
        dd_trx_params params = {{0}, 32767};
        int32_t out[32];
        int32_t peak = 0;
        dd_efm_init(&voice, (dd_efm_kind)kind);
        for (i = 0; i < 8u; ++i)
            params.control[i] = (uint16_t)((uint32_t)dd_efm_defaults_u7[kind][i] * 32767u / 127u);
        dd_efm_render(&voice, &params, 0, out, 32);
        for (i = 0; i < 32u; ++i) assert(out[i] == 0);
        for (block = 0; block < 32u; ++block) {
            dd_efm_render(&voice, &params, block == 0, out, 32);
            for (i = 0; i < 32u; ++i) {
                int32_t sample = out[i] / 65536;
                assert(sample >= -32767 && sample <= 32767);
                if (sample > peak) peak = sample;
            }
        }
        assert(peak > 100);
    }
    puts("ok: eight EFM voices trigger, render, and stay bounded");
    return 0;
}
