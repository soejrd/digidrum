/* SPDX-License-Identifier: MIT
 * Thin browser bridge. Synthesis stays in the shared C machine sources.
 */
#include <stdint.h>
#include <stddef.h>
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

typedef struct {
    const char *name;
    uint32_t offset, min, max;
} efm_tweak_desc;

#define TWEAK(field, low, high) {#field, offsetof(dd_efm_tweaks, field), low, high}
static const efm_tweak_desc tweak_table[] = {
    TWEAK(c_min_hz, 0, 2000), TWEAK(c_hz_range, 0, 5000),
    TWEAK(m_min_hz, 0, 3000), TWEAK(m_hz_range, 0, 5000),
    TWEAK(c2_min_hz, 0, 2000), TWEAK(c2_hz_range, 0, 5000),
    TWEAK(m2_offset_hz, 0, 5000), TWEAK(m2_hz_per_control, 0, 100),
    TWEAK(rim_mod_ratio, 0, 10), TWEAK(rim_mod_offset, 0, 5000),
    TWEAK(cb_ratio_percent, 0, 250), TWEAK(sweep_max_hz, 0, 4000),
    TWEAK(bd_mod_ratio_min_q8, 0, 8192),
    TWEAK(bd_mod_ratio_span_q8, 0, 8192),
    TWEAK(bd_index_max_q8, 0, 2048),
    TWEAK(bd_mod_attack_ms, 0, 200),
    TWEAK(amp_min_ms, 1, 10000), TWEAK(amp_span_ms, 0, 10000),
    TWEAK(mod_min_ms, 0, 10000), TWEAK(mod_span_ms, 0, 10000),
    TWEAK(mod_fixed_ms, 0, 10000),
    TWEAK(ramp_min_ms, 0, 1000), TWEAK(ramp_span_ms, 0, 1000),
    TWEAK(aux_min_ms, 0, 10000), TWEAK(aux_span_ms, 0, 10000),
    TWEAK(cb_aux_min_ms, 0, 1000), TWEAK(cb_aux_divisor, 0, 127),
    TWEAK(depth_mult, 0, 500), TWEAK(fb_depth_mult, 0, 100),
    TWEAK(fb_depth_fix, 0, 10000),
    TWEAK(noise_gain_mult, 0, 258), TWEAK(snap_gain_mult, 0, 258),
    TWEAK(hp_min_hz, 0, 5000), TWEAK(hp_hz_range, 0, 10000),
    TWEAK(hp_fixed_hz, 0, 5000), TWEAK(hp_frac_num, 0, 10),
    TWEAK(hp_frac_den, 0, 10),
    TWEAK(clap_max_count, 0, 10), TWEAK(clap_period, 0, 5000),
    TWEAK(trem_depth_mult, 0, 258),
    TWEAK(trem_freq_min_hz, 0, 1000), TWEAK(trem_freq_range, 0, 5000),
    TWEAK(ratio_0, 0, 5000), TWEAK(ratio_1, 0, 5000),
    TWEAK(ratio_2, 0, 5000), TWEAK(ratio_3, 0, 5000),
    TWEAK(phase_offset_q2, 0, 3)
};
#undef TWEAK

#define TWEAK_COUNT (sizeof(tweak_table) / sizeof(tweak_table[0]))

uint32_t dd_web_efm_tweak_count(void) { return TWEAK_COUNT; }
uint32_t dd_web_efm_tweak_name(uint32_t index)
{
    return index < TWEAK_COUNT ? (uint32_t)(uintptr_t)tweak_table[index].name : 0u;
}
uint32_t dd_web_efm_tweak_min(uint32_t index)
{
    return index < TWEAK_COUNT ? tweak_table[index].min : 0u;
}
uint32_t dd_web_efm_tweak_max(uint32_t index)
{
    return index < TWEAK_COUNT ? tweak_table[index].max : 0u;
}
uint32_t dd_web_efm_tweak_get(uint32_t index)
{
    const uint8_t *base = (const uint8_t *)&efm_voice.tweaks;
    if (current_kind < (int)DD_TRX_MACHINE_COUNT || index >= TWEAK_COUNT) return 0u;
    return *(const uint32_t *)(const void *)(base + tweak_table[index].offset);
}
void dd_web_efm_tweak_set(uint32_t index, uint32_t value)
{
    uint8_t *base = (uint8_t *)&efm_voice.tweaks;
    const efm_tweak_desc *desc;
    if (current_kind < (int)DD_TRX_MACHINE_COUNT || index >= TWEAK_COUNT) return;
    desc = &tweak_table[index];
    if (value < desc->min) value = desc->min;
    if (value > desc->max) value = desc->max;
    *(uint32_t *)(void *)(base + desc->offset) = value;
    efm_voice.cache.valid = 0;
}

void dd_web_init(int kind)
{
    uint32_t i;
    if (kind < DD_TRX_B2 || kind >= (int)(DD_TRX_MACHINE_COUNT + DD_EFM_MACHINE_COUNT)) kind = DD_TRX_B2;
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
