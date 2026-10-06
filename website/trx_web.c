/* SPDX-License-Identifier: MIT
 * Thin browser bridge. The synthesis stays in machines/trx_md.c.
 */
#include <stdint.h>
#include "dd_trx_md.h"

#define WEB_BLOCK 128u

static dd_trx_voice voice;
static dd_trx_params params;
static int32_t output[WEB_BLOCK];
static int pending_trigger;

void dd_web_init(int kind)
{
    uint32_t i;
    if (kind < DD_TRX_BD || kind > DD_TRX_SD) kind = DD_TRX_B2;
    dd_trx_init(&voice, (dd_trx_kind)kind);
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
        dd_trx_render(&voice, &params, pending_trigger, output, frames);
        pending_trigger = 0;
    }
    return (uint32_t)(uintptr_t)output;
}

uint32_t dd_web_capacity(void)
{
    return WEB_BLOCK;
}
