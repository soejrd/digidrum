/* SPDX-License-Identifier: MIT */
#include "dd_envelope.h"
#include "dd_fixed.h"

void dd_ahd_env_init(dd_ahd_env *e)
{
    e->value = 0;
    e->attack_coeff = 0;
    e->decay_coeff = 0;
    e->hold_count = 0;
    e->stage = DD_AHD_ATTACK;
    e->active = 0;
}

void dd_ahd_env_trigger(dd_ahd_env *e, int32_t peak, int32_t attack_coeff,
                        int32_t decay_coeff, int16_t hold_samples)
{
    (void)peak;
    e->value = 0;
    e->attack_coeff = attack_coeff;
    e->decay_coeff = decay_coeff;
    e->hold_count = hold_samples;
    e->stage = DD_AHD_ATTACK;
    e->active = 1;
}

int32_t dd_ahd_env_step(dd_ahd_env *e)
{
    if (!e->active)
        return 0;

    switch (e->stage) {
    case DD_AHD_ATTACK:
        e->value += dd_mul_q15(32767 - e->value, e->attack_coeff);
        if (e->value >= 32700) {
            e->value = 32767;
            e->stage = DD_AHD_HOLD;
        }
        break;

    case DD_AHD_HOLD:
        if (e->hold_count > 0) {
            --e->hold_count;
        } else {
            e->stage = DD_AHD_DECAY;
        }
        break;

    case DD_AHD_DECAY:
        e->value = dd_mul_q15(e->value, e->decay_coeff);
        if (e->value < 16) {
            e->value = 0;
            e->active = 0;
        }
        break;
    }

    return e->value;
}

void dd_pitch_sweep_init(dd_pitch_sweep_env *e)
{
    e->value = 0;
    e->attack_coeff = 0;
    e->decay_coeff = 0;
    e->target = 0;
    e->base = 0;
    e->active = 0;
}

void dd_pitch_sweep_trigger(dd_pitch_sweep_env *e, int32_t base,
                            int32_t target, int32_t attack_coeff,
                            int32_t decay_coeff)
{
    e->base = base;
    e->target = target;
    e->value = target - base;
    e->attack_coeff = attack_coeff;
    e->decay_coeff = decay_coeff;
    e->active = 1;
}

int32_t dd_pitch_sweep_step(dd_pitch_sweep_env *e)
{
    if (!e->active)
        return e->base;
    e->value = dd_mul_q15(e->value, e->decay_coeff);
    if (e->value >= -1 && e->value <= 1) {
        e->value = 0;
        e->active = 0;
        return e->base;
    }
    return e->base + e->value;
}
