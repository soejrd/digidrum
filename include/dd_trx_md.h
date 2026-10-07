/* SPDX-License-Identifier: MIT */
#ifndef DD_TRX_MD_H
#define DD_TRX_MD_H

#include <stdint.h>
#include "dd_machine.h"
#include "dd_osc.h"
#include "dd_envelope.h"
#include "dd_noise.h"
#include "dd_filter.h"
#include "dd_rate.h"

/* Eight machine controls plus a separate track level. All are 0..32767. */
typedef struct {
    uint16_t control[8];
    uint16_t level;
} dd_trx_params;

typedef enum {
    DD_TRX_B2,
    DD_TRX_SD
} dd_trx_kind;

/* Web kind order: B2, SD, CH, OH, CY, RS, CB, CL.
 * These 0..127 values are the sole source of fresh-machine defaults for
 * firmware and browser. The firmware range hook converts them to 8.8. */
#define DD_TRX_MACHINE_COUNT 8u
extern const uint8_t dd_trx_defaults_u7[DD_TRX_MACHINE_COUNT][8];
extern const uint8_t dd_trx_control_counts[DD_TRX_MACHINE_COUNT];

/* Browser algorithm controls. 100 preserves the firmware sound. */
typedef struct {
    uint32_t pitch_percent;
    uint32_t decay_percent;
    uint32_t transient_percent;
    uint32_t sweep_percent;
    uint32_t noise_percent;
    uint32_t body_percent;
    uint32_t metal_percent;
    uint32_t hp_eq_q_x100;
    uint32_t hp_eq_boost_percent;
    uint32_t lp_eq_q_x100;
    uint32_t lp_eq_boost_percent;
} dd_trx_algorithm;

extern const dd_trx_algorithm dd_trx_default_algorithm;

/* Control order follows the Machinedrum manual.
 * B2: PTCH DEC RAMP HOLD TICK NOIS DIRT DIST
 * SD: PTCH DEC BUMP BENV SNAP TONE TUNE CLIP */
typedef struct {
    dd_osc body;
    dd_osc second;
    dd_noise noise;
    dd_onepole noise_filter;
    dd_decay_env amp;
    dd_decay_env transient;
    dd_pitch_sweep_env bump;
    dd_downsampler rate;
    dd_trx_params last_params;
    dd_trx_algorithm algorithm;
    uint32_t body_inc;
    uint32_t second_inc;
    int32_t decay_coeff;
    int32_t bump_depth;
    int32_t bump_coeff;
    int32_t noise_gain;
    int32_t noise_filter_coeff;
    int32_t body_gain;
    int32_t distortion;
    int32_t level;
    int32_t held_sample;
    int32_t b2_prev_sample;
    /* TRX-B2 model, derived from the primed Gearmulator reference capture. */
    uint32_t b2_base_inc;
    uint32_t b2_sweep_hz_q8;
    uint32_t b2_sweep_target_q8;
    uint32_t b2_sweep_start_q8;
    uint32_t b2_age;
    uint32_t b2_short_resid_q24;
    uint32_t b2_short_resid_target_q24;
    uint32_t b2_hold_samples;
    uint16_t b2_decay_coeff;
    uint16_t b2_late_coeff;
    uint32_t b2_amp_q24;
    uint32_t b2_amp_target_q24;
    uint32_t b2_noise_env_q24;
    uint32_t b2_noise_age;
    int32_t b2_noise_lp;
    uint16_t b2_tick;
    uint16_t b2_noise;
    uint16_t b2_dirt;
    uint16_t b2_dist;
    uint16_t b2_short_resid_gain;
    dd_trx_kind kind;
    uint8_t params_valid;
} dd_trx_voice;

void dd_trx_init(dd_trx_voice *voice, dd_trx_kind kind);
void dd_trx_render(dd_trx_voice *voice, const dd_trx_params *params,
                   int trigger, int32_t *out, uint32_t size);

#endif
