/* SPDX-License-Identifier: MIT
 * Original DSP approximations of the documented TRX-BD, TRX-B2 and TRX-SD
 * controls. They do not reproduce Elektron's unpublished implementation.
 * Each voice runs at half rate and returns Q1.31 samples with ZOH output.
 * The eight machine controls are separate from the track level.
 */
#include "../include/dd_trx_md.h"
#include "../include/dd_fixed.h"

static int32_t scaled(uint16_t value, int32_t span)
{
    return ((int32_t)value * span) >> 15;
}

static int32_t decay_from_control(uint16_t control)
{
    int32_t remaining = 32767 - (int32_t)control;
    /* Curved 25 ms to multi-second contour at the 24 kHz internal rate. */
    return 32765 - ((remaining * remaining) >> 21);
}

static void update_controls(dd_trx_voice *v, const dd_trx_params *p)
{
    uint32_t i;
    uint16_t changed = 0;
    for (i = 0; i < 8; ++i) {
        if (!v->params_valid || v->last_params.control[i] != p->control[i]) {
            v->last_params.control[i] = p->control[i];
            changed |= (uint16_t)(1u << i);
        }
    }
    if (!v->params_valid || v->last_params.level != p->level) {
        v->last_params.level = p->level;
        v->level = p->level;
    }
    v->params_valid = 1;

    if (changed & (1u << 0)) {
        v->body_inc = 1000000u + (uint32_t)p->control[0] * 2000u;
        if (v->kind == DD_TRX_SD)
            v->body_inc = 3000000u + (uint32_t)p->control[0] * 2500u;
    }
    if (changed & (1u << 1)) {
        v->decay_coeff = decay_from_control(p->control[1]);
        dd_decay_env_set_coeff(&v->amp, v->decay_coeff);
        v->held_amp.decay_coeff = v->decay_coeff;
    }
    if (v->kind == DD_TRX_BD) {
        if (changed & (1u << 2))
            v->ramp_depth = scaled(p->control[2], 22000);
        if (changed & (1u << 3)) {
            v->ramp_coeff = 32000 + scaled(p->control[3], 765);
            v->ramp.decay_coeff = v->ramp_coeff;
        }
        if (changed & (1u << 4))
            v->start_gain = scaled(p->control[4], 24000);
        if (changed & (1u << 5))
            v->noise_gain = scaled(p->control[5], 18000);
        if (changed & (1u << 6))
            v->harmonic_gain = scaled(p->control[6], 15000);
        if (changed & (1u << 7))
            v->distortion = p->control[7];
        v->noise_filter_coeff = 22000;
    } else if (v->kind == DD_TRX_B2) {
        if (changed & (1u << 2))
            v->ramp_depth = scaled(p->control[2], 26000);
        v->ramp_coeff = 30500;
        if (changed & (1u << 3))
            v->hold_samples = scaled(p->control[3], 12000);
        if (changed & (1u << 4))
            v->tick_gain = scaled(p->control[4], 16000);
        if (changed & (1u << 5))
            v->noise_gain = scaled(p->control[5], 16000);
        if (changed & (1u << 6))
            v->bits = 12 - (int32_t)(((uint32_t)p->control[6] * 10u + 16383u) / 32767u);
        if (changed & (1u << 7))
            v->distortion = p->control[7];
        v->noise_filter_coeff = 10000;
    } else {
        if (changed & (1u << 2))
            v->bump_depth = scaled(p->control[2], 22000);
        if (changed & (1u << 3)) {
            v->bump_coeff = 32000 + scaled(p->control[3], 765);
            v->bump.decay_coeff = v->bump_coeff;
        }
        if (changed & (1u << 4))
            v->noise_gain = scaled(p->control[4], 26000);
        if (changed & (1u << 5)) {
            v->noise_filter_coeff = 31000 - scaled(p->control[5], 26000);
            v->body_gain = 24000 - scaled(p->control[5], 7000);
        }
        if (changed & ((1u << 0) | (1u << 6)))
            v->second_inc = v->body_inc + (v->body_inc >> 1)
                            + (uint32_t)p->control[6] * 1000u;
        if (changed & (1u << 7))
            v->distortion = p->control[7];
    }
}

static void trigger_voice(dd_trx_voice *v)
{
    dd_osc_init(&v->body);
    dd_osc_init(&v->second);
    dd_onepole_init(&v->noise_filter);
    dd_decay_env_trigger(&v->amp);
    dd_decay_env_set_coeff(&v->transient,
                           v->kind == DD_TRX_SD ? 32700 : 30000);
    dd_decay_env_trigger(&v->transient);
    dd_pitch_sweep_trigger(&v->ramp, 0, v->ramp_depth, v->ramp_coeff);
    dd_pitch_sweep_trigger(&v->bump, 0,
                           v->kind == DD_TRX_SD ? v->bump_depth : v->start_gain,
                           v->kind == DD_TRX_SD ? v->bump_coeff : 32200);
    dd_ahd_env_trigger(&v->held_amp, 32767, 32767, v->decay_coeff,
                       (int16_t)v->hold_samples);
    dd_downsampler_init(&v->rate);
    v->held_sample = 0;
}

static int32_t render_half(dd_trx_voice *v)
{
    int32_t body, noise, mixed, amp;
    int32_t transient;
    uint32_t inc;

    if (v->kind == DD_TRX_B2 ? !v->held_amp.active : !v->amp.active)
        return 0;

    transient = dd_decay_env_step(&v->transient);
    noise = dd_mul_q15(dd_noise_q15(&v->noise), v->noise_gain);
    noise = dd_onepole_hp(&v->noise_filter, noise, v->noise_filter_coeff);
    noise = dd_mul_q15(noise, transient);

    if (v->kind == DD_TRX_BD) {
        int32_t ramp = dd_pitch_sweep_step(&v->ramp);
        int32_t start = dd_pitch_sweep_step(&v->bump);
        inc = v->body_inc + (uint32_t)ramp * 800u + (uint32_t)start * 1200u;
        body = dd_osc_sine_interp(&v->body, inc);
        body = dd_mul_q15(body, 24000);
        body += dd_mul_q15(dd_osc_sine_interp(&v->second, inc * 2u),
                           v->harmonic_gain);
        mixed = dd_clamp_q15(body + noise);
        amp = dd_decay_env_step(&v->amp);
    } else if (v->kind == DD_TRX_B2) {
        int32_t ramp = dd_pitch_sweep_step(&v->ramp);
        inc = v->body_inc + (uint32_t)ramp * 800u;
        body = dd_mul_q15(dd_osc_sine_interp(&v->body, inc), 25000);
        mixed = dd_clamp_q15(body + noise +
                             dd_mul_q15(transient, v->tick_gain));
        mixed = dd_bit_quantize(mixed, v->bits);
        amp = dd_ahd_env_step(&v->held_amp);
    } else {
        int32_t bump = dd_pitch_sweep_step(&v->bump);
        inc = v->body_inc + (uint32_t)bump * 700u;
        body = dd_mul_q15(dd_osc_sine_interp(&v->body, inc), v->body_gain);
        body += dd_mul_q15(dd_osc_sine_interp(&v->second,
                            v->second_inc + (uint32_t)bump * 1000u), 10000);
        mixed = dd_clamp_q15(body + noise);
        amp = dd_decay_env_step(&v->amp);
    }
    mixed = dd_soft_clip(mixed, v->distortion);
    mixed = dd_mul_q15(mixed, amp);
    mixed = dd_mul_q15(mixed, v->level);
    return dd_clamp_q15(mixed);
}

void dd_trx_init(dd_trx_voice *v, dd_trx_kind kind)
{
    dd_osc_init(&v->body);
    dd_osc_init(&v->second);
    dd_noise_init(&v->noise, 0x6d2b79f5u + (uint32_t)kind);
    dd_onepole_init(&v->noise_filter);
    dd_decay_env_init(&v->amp);
    dd_decay_env_init(&v->transient);
    dd_pitch_sweep_init(&v->ramp);
    dd_pitch_sweep_init(&v->bump);
    dd_ahd_env_init(&v->held_amp);
    dd_downsampler_init(&v->rate);
    v->body_inc = 0;
    v->second_inc = 0;
    v->decay_coeff = 0;
    v->ramp_depth = 0;
    v->ramp_coeff = 0;
    v->bump_depth = 0;
    v->bump_coeff = 0;
    v->start_gain = 0;
    v->noise_gain = 0;
    v->harmonic_gain = 0;
    v->tick_gain = 0;
    v->noise_filter_coeff = 0;
    v->body_gain = 0;
    v->bits = 12;
    v->distortion = 0;
    v->level = 0;
    v->hold_samples = 0;
    v->held_sample = 0;
    v->kind = kind;
    v->params_valid = 0;
}

void dd_trx_render(dd_trx_voice *v, const dd_trx_params *p,
                   int trigger, int32_t *out, uint32_t size)
{
    uint32_t i;
    update_controls(v, p);
    if (trigger)
        trigger_voice(v);
    for (i = 0; i < size; ++i) {
        if (dd_downsampler_should_process(&v->rate))
            v->held_sample = render_half(v);
        out[i] = v->held_sample * 65536;
    }
}
