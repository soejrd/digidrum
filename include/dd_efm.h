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

/* Browser algorithm editor values. All fields are uint32_t so the WASM
 * bridge can enumerate them safely by offset. Defaults preserve the sound. */
typedef struct {
    uint32_t c_min_hz, c_hz_range, m_min_hz, m_hz_range;
    uint32_t c2_min_hz, c2_hz_range, m2_offset_hz, m2_hz_per_control;
    uint32_t rim_mod_ratio, rim_mod_offset, cb_ratio_percent;
    uint32_t sweep_max_hz;
    /* BD: modulator/base ratio and FM index in Q8; attack in milliseconds. */
    uint32_t bd_mod_ratio_min_q8, bd_mod_ratio_span_q8, bd_index_max_q8;
    uint32_t bd_mod_attack_ms;
    uint32_t amp_min_ms, amp_span_ms, mod_min_ms, mod_span_ms, mod_fixed_ms;
    uint32_t ramp_min_ms, ramp_span_ms, aux_min_ms, aux_span_ms;
    uint32_t cb_aux_min_ms, cb_aux_divisor;
    uint32_t depth_mult, fb_depth_mult, fb_depth_fix;
    uint32_t noise_gain_mult, snap_gain_mult;
    uint32_t hp_min_hz, hp_hz_range, hp_fixed_hz;
    uint32_t hp_frac_num, hp_frac_den;
    uint32_t clap_max_count, clap_period;
    uint32_t trem_depth_mult, trem_freq_min_hz, trem_freq_range;
    uint32_t ratio_0, ratio_1, ratio_2, ratio_3;
    uint32_t phase_offset_q2;
} dd_efm_tweaks;

extern const dd_efm_tweaks dd_efm_default_tweaks[DD_EFM_MACHINE_COUNT];

typedef struct {
    dd_osc carrier[4], modulator[4], tremolo;
    dd_decay_env amp, mod, ramp, aux;
    dd_onepole hp[2];
    dd_noise noise;
    dd_param_cache cache;
    dd_efm_tweaks tweaks;
    uint32_t carrier_inc[4], mod_inc[4];
    uint32_t age, clap_time, clap_period;
    int32_t feedback[4];
    int32_t depth, fb_depth, sweep_inc, noise_gain, snap_gain;
    int32_t hp_coeff, level, trem_depth, trem_inc;
    uint32_t bd_base_hz, bd_ratio_q8, bd_index_q8;
    uint32_t bd_mod_attack_samples, bd_mod_attack_step_q22;
    uint8_t control[8], clap_count, clap_stage, active, bd_mod_decay_started;
    dd_efm_kind kind;
} dd_efm_voice;

void dd_efm_init(dd_efm_voice *voice, dd_efm_kind kind);
void dd_efm_render(dd_efm_voice *voice, const dd_trx_params *params,
                   int trigger, int32_t *out, uint32_t size);

#endif
