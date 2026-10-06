/* SPDX-License-Identifier: MIT */
#ifndef DD_EFM_H
#define DD_EFM_H

#include "dd_trx_md.h"
#include "dd_param_cache.h"

typedef enum {
    DD_EFM_BD, DD_EFM_SD, DD_EFM_XT, DD_EFM_CP,
    DD_EFM_RS, DD_EFM_CB, DD_EFM_HH, DD_EFM_CY
} dd_efm_kind;

#define DD_EFM_MACHINE_COUNT 8u
extern const uint8_t dd_efm_defaults_u7[DD_EFM_MACHINE_COUNT][8];
extern const uint8_t dd_efm_control_counts[DD_EFM_MACHINE_COUNT];

typedef struct {
    dd_osc carrier[4], modulator[4], tremolo;
    dd_decay_env amp, mod, ramp, aux;
    dd_onepole hp[2];
    dd_noise noise;
    dd_param_cache cache;
    uint32_t carrier_inc[4], mod_inc[4];
    uint32_t age, clap_time, clap_period;
    int32_t feedback[4];
    int32_t depth, fb_depth, sweep_inc, noise_gain, snap_gain;
    int32_t hp_coeff, level, trem_depth, trem_inc;
    uint8_t control[8], clap_count, clap_stage, active;
    dd_efm_kind kind;
} dd_efm_voice;

void dd_efm_init(dd_efm_voice *voice, dd_efm_kind kind);
void dd_efm_render(dd_efm_voice *voice, const dd_trx_params *params,
                   int trigger, int32_t *out, uint32_t size);

#endif
