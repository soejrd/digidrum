/* SPDX-License-Identifier: MIT */
#ifndef DD_TRX_FAMILY_H
#define DD_TRX_FAMILY_H

#include <stdint.h>
#include "dd_trx_md.h"

/* Browser kind numbers; 0 is the separately measured TRX-B2. */
typedef enum {
    DD_TRXF_SD = 1, DD_TRXF_CH = 2, DD_TRXF_OH = 3,
    DD_TRXF_CY = 4, DD_TRXF_RS = 5, DD_TRXF_CB = 6,
    DD_TRXF_CL = 7
} dd_trx_family_kind;

typedef struct {
    dd_osc osc[6];
    dd_noise noise;
    dd_onepole filter[8];
    dd_trx_family_kind kind;
    uint32_t age;
    uint32_t inc[6];
    int32_t metal_lp_coeff;
    int32_t metal_hp_coeff;
    int32_t metal_mix;
    int32_t metal_prev;
    int32_t metal_next;
    uint32_t sweep;
    uint32_t sweep_step;
    uint32_t double_at;
    uint32_t gap_at;
    uint32_t env;
    uint32_t aux_env;
    uint32_t snap_env;
    uint32_t env_step;
    uint32_t aux_step;
    uint32_t snap_step;
    uint16_t control[8];
    uint16_t level;
    dd_trx_params last_params;
    uint8_t double_done;
    uint8_t params_valid;
    uint8_t metal_phase;
} dd_trx_family_voice;

void dd_trx_family_init(dd_trx_family_voice *voice, dd_trx_family_kind kind);
void dd_trx_family_render(dd_trx_family_voice *voice, const dd_trx_params *params,
                          int trigger, int32_t *out, uint32_t size);

#endif
