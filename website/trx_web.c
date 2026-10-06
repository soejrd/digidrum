/* SPDX-License-Identifier: MIT
 * Thin browser bridge. Synthesis stays in the shared C machine sources.
 */
#include <stdint.h>
#include "dd_trx_md.h"
#include "dd_trx_family.h"
#include "dd_efm.h"

#define WEB_BLOCK 128u

static dd_trx_voice voice;
static dd_trx_family_voice family_voice;
static dd_efm_voice efm_voice;
static dd_trx_params params;
static int32_t output[WEB_BLOCK];
static int pending_trigger;
static int current_kind;

void dd_web_init(int kind)
{
    uint32_t i;
    if (kind < DD_TRXF_BD || kind >= (int)(DD_TRX_MACHINE_COUNT + DD_EFM_MACHINE_COUNT)) kind = DD_TRX_B2;
    current_kind = kind;
    if (kind == DD_TRX_B2) dd_trx_init(&voice, DD_TRX_B2);
    else if (kind < (int)DD_TRX_MACHINE_COUNT)
        dd_trx_family_init(&family_voice, (dd_trx_family_kind)kind);
    else dd_efm_init(&efm_voice, (dd_efm_kind)(kind - DD_TRX_MACHINE_COUNT));
    for (i = 0; i < 8u; ++i) params.control[i] = 0;
    params.level = 32767;
    pending_trigger = 0;
}

void dd_web_set_control(uint32_t index, uint32_t value)
{
    if (index >= 8u) return;
    if (value > 127u) value = 127u;
    params.control[index] = (uint16_t)((value * 32767u + 63u) / 127u);
}

void dd_web_set_level(uint32_t value)
{
    if (value > 127u) value = 127u;
    params.level = (uint16_t)((value * 32767u + 63u) / 127u);
}

void dd_web_trigger(void)
{
    pending_trigger = 1;
}

uint32_t dd_web_render(uint32_t frames)
{
    if (frames > WEB_BLOCK) frames = WEB_BLOCK;
    if (frames) {
        if (current_kind == DD_TRX_B2)
            dd_trx_render(&voice, &params, pending_trigger, output, frames);
        else if (current_kind < (int)DD_TRX_MACHINE_COUNT)
            dd_trx_family_render(&family_voice, &params, pending_trigger, output, frames);
        else dd_efm_render(&efm_voice, &params, pending_trigger, output, frames);
        pending_trigger = 0;
    }
    return (uint32_t)(uintptr_t)output;
}

uint32_t dd_web_capacity(void)
{
    return WEB_BLOCK;
}

uint32_t dd_web_machine_count(void)
{
    return DD_TRX_MACHINE_COUNT + DD_EFM_MACHINE_COUNT;
}

uint32_t dd_web_control_count(uint32_t kind)
{
    if (kind < DD_TRX_MACHINE_COUNT) return dd_trx_control_counts[kind];
    kind -= DD_TRX_MACHINE_COUNT;
    return kind < DD_EFM_MACHINE_COUNT ? dd_efm_control_counts[kind] : 0u;
}

uint32_t dd_web_default_control(uint32_t kind, uint32_t index)
{
    if (index >= 8u) return 0u;
    if (kind < DD_TRX_MACHINE_COUNT) return dd_trx_defaults_u7[kind][index];
    kind -= DD_TRX_MACHINE_COUNT;
    return kind < DD_EFM_MACHINE_COUNT ? dd_efm_defaults_u7[kind][index] : 0u;
}
