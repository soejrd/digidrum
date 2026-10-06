/* SPDX-License-Identifier: MIT
 * TRX-B2 is a new black-box model from the primed reference sweep.
 * TRX-SD retains its earlier provisional model.
 * The legacy voices use half-rate ZOH; TRX-B2 interpolates its half-rate
 * body to the 48 kHz output to avoid held-sample imaging.
 * The eight machine controls are separate from the track level.
 */
#include "../include/dd_trx_md.h"

const uint8_t dd_trx_defaults_u7[DD_TRX_MACHINE_COUNT][8] = {
    {64, 64, 64, 0, 0, 0, 0, 0},       /* B2 */
    {34, 13, 0, 64, 127, 0, 104, 85}, /* SD: Gearmulator baseline */
    {2, 10, 0, 127, 0, 0, 0, 0},      /* CH */
    {2, 10, 0, 127, 0, 0, 0, 0},      /* OH */
    {127, 32, 94, 105, 127, 2, 0, 0}, /* CY */
    {64, 64, 0, 0, 0, 0, 0, 0},       /* RS: provisional */
    {64, 64, 64, 64, 0, 0, 0, 0},     /* CB: provisional */
    {64, 64, 0, 64, 64, 0, 0, 0}     /* CL: provisional */
};

const uint8_t dd_trx_control_counts[DD_TRX_MACHINE_COUNT] =
    {8, 8, 5, 5, 6, 3, 8, 6};
#include "../include/dd_fixed.h"
#include "../include/dd_tables.h"

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

typedef struct { uint8_t value; uint16_t amount; } b2_knot;

/* Late pitch measured from PTCH 0..127 at 200-500 ms, in Hz. */
static const b2_knot b2_pitch[] = {
    {0, 10}, {25, 24}, {51, 41}, {64, 50},
    {76, 60}, {102, 82}, {127, 107}
};
/* Exponential amplitude time constants inferred from the DEC sweep, ms. */
static const b2_knot b2_decay_ms[] = {
    {0, 4}, {25, 7}, {51, 30}, {64, 74},
    {76, 175}, {102, 960}, {127, 5300}
};
/* HOLD delays the DEC release; these values follow its T40 displacement. */
static const b2_knot b2_hold_ms[] = {
    {0, 0}, {25, 20}, {51, 85}, {64, 190},
    {76, 400}, {102, 1980}, {127, 9530}
};

static uint16_t b2_control(uint16_t value)
{
    return (uint16_t)(((uint32_t)value * 127u + 16383u) / 32767u);
}

static uint32_t b2_lookup(const b2_knot *knots, uint32_t size, uint16_t control)
{
    uint32_t i;
    for (i = 1; i < size; ++i) {
        if (control <= knots[i].value) {
            uint32_t left = knots[i-1].amount;
            uint32_t right = knots[i].amount;
            return left + (right - left) * (control - knots[i-1].value) /
                (knots[i].value - knots[i-1].value);
        }
    }
    return knots[size-1].amount;
}

static uint16_t b2_block_decay(uint32_t tau_ms)
{
    uint32_t loss = 1048576u / (tau_ms * 24u);
    if (loss > 32767u) loss = 32767u;
    if (loss < 1u) loss = 1u;
    return (uint16_t)(32768u - loss);
}

static uint32_t b2_mul_q15_u24(uint32_t value, uint16_t coeff)
{
    return (value >> 15) * coeff + ((value & 32767u) * coeff >> 15);
}

static int32_t b2_symmetric_quantize(int32_t sample, int32_t bits)
{
    int32_t step = 1 << (16 - bits);
    int32_t magnitude = dd_abs32(sample);
    int32_t rounded = ((magnitude + step / 2) / step) * step;
    return sample < 0 ? -rounded : rounded;
}

/* Keep the clean B2 body above 16-bit precision through its long decay. */
static int32_t b2_sine_q31(dd_osc *osc, uint32_t inc)
{
    uint32_t phase, index, fraction;
    int32_t first, next;
    osc->phase += inc;
    phase = osc->phase;
    index = phase >> (32u - DD_SINE_SIZE_LOG2);
    fraction = (phase & ((1u << (32u - DD_SINE_SIZE_LOG2)) - 1u)) >> 8;
    first = dd_sine_tab[index & (DD_SINE_SIZE - 1u)];
    next = dd_sine_tab[(index + 1u) & (DD_SINE_SIZE - 1u)];
    return first * 65536 + (next - first) * (int32_t)fraction * 2;
}

static int32_t b2_mul_q15_s31(int32_t value, int32_t coeff)
{
    return (value >> 16) * coeff * 2 +
        (int32_t)(((uint32_t)value & 65535u) * (uint32_t)coeff >> 14);
}

static int32_t b2_mul_q24_s31(int32_t value, uint32_t coeff)
{
    int32_t high = value >> 12;
    int32_t low = (int32_t)((uint32_t)value & 4095u);
    int32_t upper = (int32_t)(coeff >> 12);
    int32_t lower = (int32_t)(coeff & 4095u);
    return high * upper + ((high * lower) >> 12) +
        ((low * upper) >> 12);
}

static void b2_update(dd_trx_voice *v, const dd_trx_params *p)
{
    uint16_t pitch = b2_control(p->control[0]);
    uint16_t decay = b2_control(p->control[1]);
    uint16_t ramp = b2_control(p->control[2]);
    uint16_t hold = b2_control(p->control[3]);
    uint32_t tau_ms = b2_lookup(b2_decay_ms, 7, decay);
    uint32_t hold_ms = b2_lookup(b2_hold_ms, 7, hold);
    v->b2_base_inc = b2_lookup(b2_pitch, 7, pitch) * 178957u;
    v->b2_sweep_start_q8 = (uint32_t)ramp * 870u; /* 3.40 Hz/control */
    v->b2_decay_coeff = b2_block_decay(tau_ms);
    v->b2_late_coeff = b2_block_decay(tau_ms / 2u);
    v->b2_hold_samples = hold_ms * 24u;
    v->b2_tick = b2_control(p->control[4]);
    v->b2_noise = b2_control(p->control[5]);
    v->b2_dirt = b2_control(p->control[6]);
    v->b2_dist = b2_control(p->control[7]);
    v->b2_short_resid_gain = decay <= 25u ?
        (uint16_t)(400u - (uint32_t)decay * 8u) : 0;
}

static int32_t b2_render_half(dd_trx_voice *v)
{
    int32_t body, mixed;
    uint32_t phase_inc;
    uint32_t amp, residual, sweep;
    if (v->b2_amp_q24 == 0 && v->b2_short_resid_q24 == 0) return 0;
    if ((v->b2_age & 31u) == 0u) {
        uint16_t coeff = 32766; /* Measured slow loss during HOLD (~20 s). */
        if (v->b2_age > v->b2_hold_samples) {
            coeff = v->b2_decay_coeff;
            if (b2_control(v->last_params.control[1]) >= 120u &&
                v->b2_age - v->b2_hold_samples >
                    (v->b2_hold_samples ? 288000u : 336000u))
                coeff = v->b2_late_coeff;
        }
        v->b2_amp_target_q24 = b2_mul_q15_u24(v->b2_amp_q24, coeff);
        if (v->b2_short_resid_gain)
            v->b2_short_resid_target_q24 =
                b2_mul_q15_u24(v->b2_short_resid_q24, 31540u);
    }
    if ((v->b2_age & 15u) == 0u)
        v->b2_sweep_target_q8 =
            (v->b2_sweep_hz_q8 * 32252u) >> 15;
    sweep = v->b2_sweep_hz_q8 -
        (v->b2_sweep_hz_q8 - v->b2_sweep_target_q8) *
        (v->b2_age & 15u) / 16u;
    amp = v->b2_amp_q24 -
        (v->b2_amp_q24 - v->b2_amp_target_q24) *
        (v->b2_age & 31u) / 32u;
    residual = v->b2_short_resid_q24 -
        (v->b2_short_resid_q24 - v->b2_short_resid_target_q24) *
        (v->b2_age & 31u) / 32u;
    phase_inc = v->b2_base_inc + sweep * 699u;
    body = b2_mul_q15_s31(b2_sine_q31(&v->body, phase_inc), 8500);
    if (v->b2_dirt) {
        int32_t dry = body >> 16, low, high;
        uint16_t position, width;
        int32_t gain_milli = v->b2_dirt <= 76u ?
            1000 + (int32_t)v->b2_dirt * 30 / 76 :
            v->b2_dirt <= 102u ?
            1030 - ((int32_t)v->b2_dirt - 76) * 30 / 26 :
            1000 - ((int32_t)v->b2_dirt - 102) * 45 / 25;
        if (v->b2_dirt <= 25u) {
            low = dry;
            high = b2_symmetric_quantize(dry, 10);
            position = v->b2_dirt;
            width = 25;
        } else if (v->b2_dirt <= 51u) {
            low = b2_symmetric_quantize(dry, 10);
            high = b2_symmetric_quantize(dry, 7);
            position = v->b2_dirt - 25u;
            width = 26;
        } else if (v->b2_dirt <= 76u) {
            low = b2_symmetric_quantize(dry, 7);
            high = b2_symmetric_quantize(dry, 5);
            position = v->b2_dirt - 51u;
            width = 25;
        } else if (v->b2_dirt <= 102u) {
            low = b2_symmetric_quantize(dry, 5);
            high = b2_symmetric_quantize(dry, 4);
            position = v->b2_dirt - 76u;
            width = 26;
        } else {
            low = b2_symmetric_quantize(dry, 4);
            high = b2_symmetric_quantize(dry, 3);
            position = v->b2_dirt - 102u;
            width = 25;
        }
        body = low + (high - low) * position / width;
        body = body * gain_milli / 1000 * 65536;
    }
    if (v->b2_dist) {
        int32_t drive = (int32_t)v->b2_dist;
        int32_t gain_milli = 1000 + 115 * drive + 2 * drive * drive;
        int32_t driven = (body >> 16) * gain_milli / 1000;
        body = dd_hard_clip(driven, 8600 + 2 * drive) * 65536;
    }
    mixed = b2_mul_q24_s31(body, amp);
    if (v->b2_age < 8u && v->b2_tick) {
        int32_t impulse = v->b2_tick <= 25u ?
            (int32_t)v->b2_tick * 428 :
            10700 + ((int32_t)v->b2_tick - 25) * 250;
        if (impulse > 17200) impulse = 17200;
        mixed += (impulse * (int32_t)(8u - v->b2_age) / 8) * 65536;
    }
    if (v->b2_short_resid_gain)
        mixed += b2_mul_q24_s31(v->b2_short_resid_gain * 65536, residual);
    ++v->b2_age;
    if ((v->b2_age & 15u) == 0u)
        v->b2_sweep_hz_q8 = v->b2_sweep_target_q8;
    if ((v->b2_age & 31u) == 0u) {
        v->b2_amp_q24 = v->b2_amp_target_q24;
        v->b2_short_resid_q24 = v->b2_short_resid_target_q24;
    }
    return b2_mul_q15_s31(dd_clamp(mixed, -17300 * 65536,
                                    17300 * 65536), v->level);
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
    }
    if (v->kind == DD_TRX_B2) {
        if (changed) b2_update(v, p);
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
    if (v->kind == DD_TRX_B2) {
        dd_osc_init(&v->body);
        v->b2_sweep_hz_q8 = v->b2_sweep_start_q8;
        v->b2_sweep_target_q8 = v->b2_sweep_start_q8;
        v->b2_amp_q24 = 1u << 24;
        v->b2_amp_target_q24 = v->b2_amp_q24;
        v->b2_short_resid_q24 = v->b2_short_resid_gain ? 1u << 24 : 0;
        v->b2_short_resid_target_q24 = v->b2_short_resid_q24;
        v->b2_age = 0;
        dd_downsampler_init(&v->rate);
        v->held_sample = 0;
        v->b2_prev_sample = 0;
        v->b2_noise_env_q24 = 1u << 24;
        v->b2_noise_age = 0;
        v->b2_noise_lp = 0;
        return;
    }
    dd_osc_init(&v->body);
    dd_osc_init(&v->second);
    dd_onepole_init(&v->noise_filter);
    dd_decay_env_trigger(&v->amp);
    dd_decay_env_set_coeff(&v->transient,
                           v->kind == DD_TRX_SD ? 32700 : 30000);
    dd_decay_env_trigger(&v->transient);
    dd_pitch_sweep_trigger(&v->bump, 0, v->bump_depth, v->bump_coeff);
    dd_downsampler_init(&v->rate);
    v->held_sample = 0;
    v->b2_prev_sample = 0;
}

static int32_t render_half(dd_trx_voice *v)
{
    int32_t body, noise, mixed, amp;
    int32_t transient;
    uint32_t inc;

    if (v->kind == DD_TRX_B2) return b2_render_half(v);
    if (!v->amp.active)
        return 0;

    transient = dd_decay_env_step(&v->transient);
    noise = dd_mul_q15(dd_noise_q15(&v->noise), v->noise_gain);
    noise = dd_onepole_hp(&v->noise_filter, noise, v->noise_filter_coeff);
    noise = dd_mul_q15(noise, transient);

    {
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
    dd_pitch_sweep_init(&v->bump);
    dd_downsampler_init(&v->rate);
    v->body_inc = 0;
    v->second_inc = 0;
    v->decay_coeff = 0;
    v->bump_depth = 0;
    v->bump_coeff = 0;
    v->noise_gain = 0;
    v->noise_filter_coeff = 0;
    v->body_gain = 0;
    v->distortion = 0;
    v->level = 0;
    v->held_sample = 0;
    v->b2_base_inc = 0;
    v->b2_sweep_hz_q8 = 0;
    v->b2_sweep_target_q8 = 0;
    v->b2_sweep_start_q8 = 0;
    v->b2_age = 0;
    v->b2_short_resid_q24 = 0;
    v->b2_short_resid_target_q24 = 0;
    v->b2_hold_samples = 0;
    v->b2_decay_coeff = 0;
    v->b2_late_coeff = 0;
    v->b2_amp_q24 = 0;
    v->b2_amp_target_q24 = 0;
    v->b2_noise_env_q24 = 0;
    v->b2_noise_age = 0;
    v->b2_noise_lp = 0;
    v->b2_tick = 0;
    v->b2_noise = 0;
    v->b2_dirt = 0;
    v->b2_dist = 0;
    v->b2_short_resid_gain = 0;
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
        if (dd_downsampler_should_process(&v->rate)) {
            v->b2_prev_sample = v->held_sample;
            v->held_sample = render_half(v);
            out[i] = v->kind == DD_TRX_B2 ?
                v->b2_prev_sample / 2 + v->held_sample / 2 :
                v->held_sample * 65536;
        } else {
            out[i] = v->kind == DD_TRX_B2 ?
                v->held_sample : v->held_sample * 65536;
        }
        if (v->kind == DD_TRX_B2 && v->b2_noise &&
            v->b2_noise_env_q24 > 256u) {
            int32_t white = dd_noise_q15(&v->noise);
            int32_t gain = (int32_t)v->b2_noise * 13000 / 127;
            int32_t shaped, noise, mixed;
            v->b2_noise_lp += dd_mul_q15(white - v->b2_noise_lp, 21300);
            shaped = dd_mul_q15(v->b2_noise_lp, gain);
            noise = dd_mul_q15(shaped, (int32_t)(v->b2_noise_env_q24 >> 9));
            mixed = out[i] + dd_mul_q15(noise, v->level) * 65536;
            out[i] = dd_clamp(mixed, -17300 * 65536, 17300 * 65536);
            ++v->b2_noise_age;
            if ((v->b2_noise_age & 15u) == 0u)
                v->b2_noise_env_q24 =
                    b2_mul_q15_u24(v->b2_noise_env_q24, 31575u);
        }
    }
}
