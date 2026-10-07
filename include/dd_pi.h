/* SPDX-License-Identifier: MIT */
#ifndef DD_PI_H
#define DD_PI_H
#include <stdint.h>
#include "dd_trx_md.h"

/* PI-MT is an extra mid-tom variation; it is not a stock Machinedrum name. */
typedef enum { DD_PI_BD, DD_PI_SD, DD_PI_XT, DD_PI_MT,
               DD_PI_RS, DD_PI_ML, DD_PI_MA } dd_pi_kind;
#define DD_PI_COUNT 7u
extern const uint8_t dd_pi_defaults_u7[DD_PI_COUNT][8];
extern const uint8_t dd_pi_control_counts[DD_PI_COUNT];

typedef struct {
    dd_osc mode[4];
    dd_noise noise;
    dd_onepole noise_hp;
    dd_decay_env amp, rattle, grain;
    dd_param_cache cache;
    uint32_t inc[4];
    uint32_t age, grain_clock;
    uint16_t control[8], level;
    int32_t pitch_bend, body_damp, strike, grain_gain;
    dd_pi_kind kind;
} dd_pi_voice;

void dd_pi_init(dd_pi_voice *v, dd_pi_kind kind);
void dd_pi_render(dd_pi_voice *v, const dd_trx_params *p, int trigger,
                  int32_t *out, uint32_t size);
#endif
