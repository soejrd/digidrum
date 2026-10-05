/* SPDX-License-Identifier: MIT */
#ifndef DD_ENVELOPE_H
#define DD_ENVELOPE_H

#include <stdint.h>
#include "dd_fixed.h"

typedef struct {
    int32_t value;
    int32_t coeff;
    uint8_t active;
} dd_decay_env;

static inline void dd_decay_env_init(dd_decay_env *e)
{
    e->value = 0;
    e->coeff = 0;
    e->active = 0;
}

static inline void dd_decay_env_set_coeff(dd_decay_env *e, int32_t coeff)
{
    e->coeff = coeff;
}

static inline void dd_decay_env_trigger(dd_decay_env *e)
{
    e->value = 32767;
    e->active = 1;
}

static inline void dd_decay_env_release(dd_decay_env *e)
{
    e->active = 0;
}

static inline int32_t dd_decay_env_step(dd_decay_env *e)
{
    if (!e->active)
        return 0;
    e->value = dd_mul_q15(e->value, e->coeff);
    if (e->value < 16) {
        e->value = 0;
        e->active = 0;
    }
    return e->value;
}

typedef enum {
    DD_AHD_ATTACK,
    DD_AHD_HOLD,
    DD_AHD_DECAY
} dd_ahd_stage;

typedef struct {
    int32_t value;
    int32_t attack_coeff;
    int32_t decay_coeff;
    int16_t hold_count;
    uint8_t stage;
    uint8_t active;
} dd_ahd_env;

void dd_ahd_env_init(dd_ahd_env *e);
void dd_ahd_env_trigger(dd_ahd_env *e, int32_t peak, int32_t attack_coeff,
                        int32_t decay_coeff, int16_t hold_samples);
int32_t dd_ahd_env_step(dd_ahd_env *e);

typedef struct {
    int32_t value;
    int32_t attack_coeff;
    int32_t decay_coeff;
    int32_t target;
    int32_t base;
    uint8_t active;
} dd_pitch_sweep_env;

void dd_pitch_sweep_init(dd_pitch_sweep_env *e);
void dd_pitch_sweep_trigger(dd_pitch_sweep_env *e, int32_t base,
                            int32_t target, int32_t attack_coeff,
                            int32_t decay_coeff);
int32_t dd_pitch_sweep_step(dd_pitch_sweep_env *e);

#endif
