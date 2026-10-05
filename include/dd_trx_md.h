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
    DD_TRX_BD,
    DD_TRX_B2,
    DD_TRX_SD
} dd_trx_kind;

/* Control order follows the Machinedrum manual.
 * BD: PTCH DEC RAMP RDEC STRT NOIS HARM CLIP
 * B2: PTCH DEC RAMP HOLD TICK NOIS DIRT DIST
 * SD: PTCH DEC BUMP BENV SNAP TONE TUNE CLIP */
typedef struct {
    dd_osc body;
    dd_osc second;
    dd_noise noise;
    dd_onepole noise_filter;
    dd_decay_env amp;
    dd_decay_env transient;
    dd_pitch_sweep_env ramp;
    dd_pitch_sweep_env bump;
    dd_ahd_env held_amp;
    dd_downsampler rate;
    dd_trx_params last_params;
    uint32_t body_inc;
    uint32_t second_inc;
    int32_t decay_coeff;
    int32_t ramp_depth;
    int32_t ramp_coeff;
    int32_t bump_depth;
    int32_t bump_coeff;
    int32_t start_gain;
    int32_t noise_gain;
    int32_t harmonic_gain;
    int32_t tick_gain;
    int32_t noise_filter_coeff;
    int32_t body_gain;
    int32_t bits;
    int32_t distortion;
    int32_t level;
    int32_t hold_samples;
    int32_t held_sample;
    dd_trx_kind kind;
    uint8_t params_valid;
} dd_trx_voice;

void dd_trx_init(dd_trx_voice *voice, dd_trx_kind kind);
void dd_trx_render(dd_trx_voice *voice, const dd_trx_params *params,
                   int trigger, int32_t *out, uint32_t size);

#endif
