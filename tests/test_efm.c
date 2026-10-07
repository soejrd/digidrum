/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "dd_efm.h"

static void test_bd_signal_path(void)
{
    dd_efm_voice dry, swept, modulated;
    dd_trx_params params = {{0}, 32767};
    int32_t out;
    int32_t block_out[32];
    uint32_t i;
    for (i = 0; i < 8u; ++i)
        params.control[i] = (uint16_t)((uint32_t)dd_efm_defaults_u7[DD_EFM_BD][i] * 32767u / 127u);
    params.control[4] = 0; /* MOD off isolates the pitch path. */
    params.control[7] = 0; /* MFB off isolates the modulator frequency. */
    params.control[2] = 0;
    dd_efm_init(&dry, DD_EFM_BD);
    dd_efm_render(&dry, &params, 1, &out, 1);

    params.control[2] = 32767;
    dd_efm_init(&swept, DD_EFM_BD);
    dd_efm_render(&swept, &params, 1, &out, 1);
    assert(swept.carrier[0].phase != dry.carrier[0].phase);
    assert(swept.modulator[0].phase != dry.modulator[0].phase);

    params.control[2] = 0;
    dd_trx_params dry_params = params;
    params.control[4] = 32767;
    dd_efm_init(&modulated, DD_EFM_BD);
    dd_efm_render(&modulated, &params, 1, &out, 1);
    assert(modulated.modulator[0].phase == dry.modulator[0].phase);
    assert(!modulated.bd_mod_decay_started);
    for (i = 0; i < 120u; ++i) {
        dd_efm_render(&dry, &dry_params, 0, block_out, 32);
        dd_efm_render(&modulated, &params, 0, block_out, 32);
    }
    assert(modulated.bd_mod_decay_started);
    assert(modulated.modulator[0].phase == dry.modulator[0].phase);
    assert(modulated.carrier[0].phase != dry.carrier[0].phase);
}

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
    test_bd_signal_path();
    puts("ok: eight EFM voices trigger, render, and stay bounded");
    return 0;
}
