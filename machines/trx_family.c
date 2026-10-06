/* SPDX-License-Identifier: MIT
 * Source-informed TRX prototypes. Fixed 48 kHz, signed Q15 signal path,
 * Q24 envelopes, and Q31 output. No heap, float, or 64-bit arithmetic.
 * Topologies follow Resources/TRX.md and md-drum-synth; tuning is provisional.
 */
#include "../include/dd_trx_family.h"
#include "../include/dd_fixed.h"

#define RATE 48000u
#define PHASE_HZ 89478u /* round(2^32 / 48000) */
#define ENV_ONE 16777215u

static uint32_t hz_inc(uint32_t hz) { return hz * PHASE_HZ; }
static uint32_t ctl(const dd_trx_family_voice *v, uint32_t i)
{
    return (uint32_t)v->control[i] >> 8;
}
static uint32_t decay_step(uint32_t ms)
{
    /* Linear envelope reaches zero after approximately the requested duration. */
    uint32_t samples = ms * 48u;
    if (samples < 1u) samples = 1u;
    return ENV_ONE / samples;
}
static uint32_t decay(uint32_t value, uint32_t step)
{
    return value > step ? value - step : 0u;
}
static int32_t mul_env(int32_t signal, uint32_t env)
{
    return dd_mul_q15(signal, (int32_t)(env >> 9));
}
static int32_t soft(int32_t x, uint32_t amount)
{
    int32_t y = x + (x * (int32_t)amount) / 85;
    return dd_clamp_q15(y);
}
static void metal_rates(dd_trx_family_voice *v, uint32_t spread,
                        uint32_t size, uint32_t rate_scale)
{
    static const uint16_t base[6] = {306, 512, 551, 743, 826, 900};
    uint32_t i;
    for (i = 0; i < 6u; ++i) {
        uint32_t freq = ((uint32_t)base[i] + (i & 1u ? spread : 0u)) *
                        (160u - size / 2u) / 128u;
        v->inc[i] = hz_inc(freq) * rate_scale;
    }
}
static int32_t metal(dd_trx_family_voice *v)
{
    int32_t sum = 0;
    uint32_t i;
    for (i = 0; i < 6u; ++i)
        sum += dd_osc_square_fast(&v->osc[i], v->inc[i], 0x80000000u) >> 3;
    return dd_clamp_q15(sum);
}

/* At half rate, square the 48 kHz one-pole memory coefficient. */
static int32_t half_coeff(int32_t coeff)
{
    return dd_mul_q15(coeff, coeff);
}

void dd_trx_family_init(dd_trx_family_voice *v, dd_trx_family_kind kind)
{
    uint32_t i;
    for (i = 0; i < 6u; ++i) dd_osc_init(&v->osc[i]);
    for (i = 0; i < 8u; ++i) dd_onepole_init(&v->filter[i]);
    dd_noise_init(&v->noise, 0x731C92A5u + (uint32_t)kind);
    v->kind = kind;
    v->age = v->sweep = v->sweep_step = 0;
    v->double_at = v->gap_at = 0;
    v->env = v->aux_env = v->snap_env = 0;
    v->env_step = v->aux_step = v->snap_step = 0;
    for (i = 0; i < 6u; ++i) v->inc[i] = 0;
    for (i = 0; i < 8u; ++i) v->control[i] = 0;
    v->level = 32767;
    v->double_done = 0;
    v->params_valid = 0;
    v->metal_phase = 0;
    v->metal_prev = v->metal_next = 0;
    v->metal_lp_coeff = v->metal_hp_coeff = v->metal_mix = 0;
}

static void configure(dd_trx_family_voice *v, const dd_trx_params *p, int trigger)
{
    uint32_t i, pitch, length;
    for (i = 0; i < 8u; ++i) v->control[i] = p->control[i];
    v->level = p->level;
    pitch = ctl(v, 0);
    length = ctl(v, 1);
    switch (v->kind) {
    case DD_TRXF_BD:
        v->inc[0] = hz_inc(38u + pitch * 2u);
        v->inc[1] = v->inc[0] * 2u;
        v->env_step = decay_step(70u + length * 10u);
        v->aux_step = decay_step(4u + ctl(v, 3) * 3u);
        v->snap_step = decay_step(2u + ctl(v, 5) / 8u);
        if (trigger) v->sweep = hz_inc(ctl(v, 2) * 8u);
        v->sweep_step = hz_inc(ctl(v, 2) * 8u) / (80u + ctl(v, 3) * 28u);
        break;
    case DD_TRXF_SD:
        /* PTCH moves the filtered noise; the two tonal pitches are separate. */
        v->inc[0] = hz_inc(180u);
        v->inc[1] = hz_inc(220u + ctl(v, 6));
        v->env_step = decay_step(45u + length * 5u);
        v->aux_step = decay_step(80u + length * 8u);
        v->snap_step = decay_step(3u + length / 3u);
        if (trigger) v->sweep = hz_inc(ctl(v, 2) * 3u);
        v->sweep_step = hz_inc(ctl(v, 2) * 3u) /
                        (150u + ctl(v, 3) * 32u);
        break;
    case DD_TRXF_CH:
    case DD_TRXF_OH:
        v->env_step = decay_step((v->kind == DD_TRXF_CH ? 18u : 130u) + length *
                                 (v->kind == DD_TRXF_CH ? 4u : 18u));
        v->aux_step = decay_step(8u + length);
        /* Appendix B holds the amplitude before its linear decay. */
        v->gap_at = (v->kind == DD_TRXF_CH ? ctl(v, 0) :
                     ctl(v, 0) * 2u) * 48u;
        v->metal_lp_coeff = half_coeff(22500 - (int32_t)ctl(v, 3) * 110);
        v->metal_hp_coeff = half_coeff(26000 - (int32_t)ctl(v, 2) * 50);
        v->metal_mix = (int32_t)ctl(v, 4);
        metal_rates(v, ctl(v, 4) * 2u, 64u, 2u);
        break;
    case DD_TRXF_CY:
        v->env_step = decay_step(180u + length * 25u);
        v->aux_step = decay_step(15u + length * 8u);
        metal_rates(v, ctl(v, 0) * 2u, ctl(v, 4), 1u);
        break;
    case DD_TRXF_RS:
        v->inc[0] = hz_inc(260u + pitch * 5u);
        v->inc[1] = hz_inc(1800u);
        v->env_step = decay_step(20u + length * 4u);
        v->aux_step = decay_step(12u);
        v->snap_step = decay_step(2u);
        break;
    case DD_TRXF_CB:
        v->inc[0] = hz_inc(340u + pitch * 4u);
        v->inc[1] = hz_inc(520u + pitch * 6u);
        v->env_step = decay_step(20u + length * 5u);
        v->snap_step = decay_step(3u + ctl(v, 4));
        if (trigger) v->sweep = hz_inc(ctl(v, 4) * 5u);
        v->sweep_step = hz_inc(ctl(v, 4) * 5u) / 600u;
        break;
    case DD_TRXF_CL:
        v->inc[0] = hz_inc(1400u + pitch * 16u);
        v->inc[1] = hz_inc(1800u + pitch * 16u + ctl(v, 4) * 5u);
        v->env_step = decay_step(10u + length * 3u);
        v->aux_step = decay_step(8u + length * 2u);
        v->snap_step = decay_step(2u);
        v->double_at = 8u * 48u;
        break;
    default: break;
    }
}

static int32_t hat_half_sample(dd_trx_family_voice *v)
{
    int32_t x, n;
    uint32_t j;
    v->age += 2u;
    if (v->age > v->gap_at)
        v->env = decay(v->env, v->env_step * 2u);
    n = dd_noise_q15(&v->noise);
    x = metal(v);
    x = (x * (64 + v->metal_mix) + n * (128 - v->metal_mix)) >> 8;
    x -= v->filter[3].lp >> 4;
    x += v->filter[7].hp >> 5;
    x = dd_clamp_q15(x);
    for (j = 0; j < 4u; ++j)
        x = dd_onepole_lp_fast(&v->filter[j], x, v->metal_lp_coeff);
    for (j = 4u; j < 6u; ++j)
        x = dd_clamp_q15(dd_onepole_hp_fast(&v->filter[j], x, 25665));
    for (j = 6u; j < 8u; ++j)
        x = dd_clamp_q15(dd_onepole_hp_fast(&v->filter[j], x, v->metal_hp_coeff));
    return mul_env(x, v->env);
}

static int32_t sample(dd_trx_family_voice *v)
{
    int32_t a = 0, b = 0, n, x, y;
    uint32_t pitch_inc;
    v->age++;
    if ((v->kind != DD_TRXF_CH && v->kind != DD_TRXF_OH) || v->age > v->gap_at)
        v->env = decay(v->env, v->env_step);
    v->aux_env = decay(v->aux_env, v->aux_step);
    v->snap_env = decay(v->snap_env, v->snap_step);
    if (v->sweep) v->sweep = decay(v->sweep, v->sweep_step);
    n = dd_noise_q15(&v->noise);
    switch (v->kind) {
    case DD_TRXF_BD:
        pitch_inc = v->inc[0] + v->sweep;
        a = dd_osc_sine(&v->osc[0], pitch_inc);
        b = dd_osc_sine(&v->osc[1], pitch_inc * 2u);
        x = mul_env(a, v->env) * 3 / 4;
        x += mul_env(b, v->env) * (int32_t)ctl(v, 6) / 512;
        x += mul_env(n, v->snap_env) * (int32_t)ctl(v, 5) / 512;
        x += mul_env(a, v->aux_env) * (int32_t)ctl(v, 4) / 512;
        return soft(x, ctl(v, 7));
    case DD_TRXF_SD:
        a = dd_osc_sine(&v->osc[0], v->inc[0] + v->sweep);
        b = dd_osc_sine(&v->osc[1], v->inc[1] + v->sweep);
        x = mul_env(a, v->env) * 2 / 3;
        x += mul_env(b, v->env) * (int32_t)ctl(v, 5) / 256;
        /* Narrow moving noise band: PTCH changes no tonal oscillator. */
        y = dd_onepole_hp(&v->filter[0], n, 28500 - (int32_t)ctl(v, 0) * 70);
        y = dd_onepole_lp(&v->filter[1], y, 25800 - (int32_t)ctl(v, 0) * 70);
        x += mul_env(y, v->aux_env) * (int32_t)ctl(v, 4) / 128;
        x += mul_env(y, v->snap_env) * (int32_t)ctl(v, 4) / 384;
        return soft(x, ctl(v, 7));
    case DD_TRXF_CY:
        a = metal(v);
        x = (a * 3 + n) / 4;
        b = dd_onepole_hp(&v->filter[0], x, 14000 + (int32_t)ctl(v, 5) * 100);
        y = dd_onepole_hp(&v->filter[1], x, 24000 - (int32_t)ctl(v, 3) * 130);
        x = mul_env(b, v->env) * 3 / 4 + mul_env(y, v->aux_env) * (int32_t)ctl(v, 2) / 256;
        return soft(x, ctl(v, 7));
    case DD_TRXF_RS:
        /* DIST frequency-modulates and clips only the decaying body. */
        b = dd_osc_sine(&v->osc[1], v->inc[1]);
        a = dd_osc_sine(&v->osc[0], v->inc[0] +
                        (uint32_t)((b * (int32_t)ctl(v, 2) / 256) * 3000));
        x = soft(mul_env(a, v->env) * 2 / 3, ctl(v, 2));
        x += mul_env(b, v->aux_env) / 3;
        return dd_clamp_q15(x);
    case DD_TRXF_CB:
        a = dd_osc_square(&v->osc[0], v->inc[0] + v->sweep, 0x40000000u);
        b = dd_osc_square(&v->osc[1], v->inc[1] + v->sweep, 0x40000000u);
        x = (a + b) / 2;
        x = dd_onepole_hp(&v->filter[0], x, 18000 + (int32_t)ctl(v, 3) * 80);
        x = dd_onepole_lp(&v->filter[1], x, 14000 + (int32_t)ctl(v, 3) * 95);
        x = mul_env(x, v->env) * (int32_t)(128u + ctl(v, 2)) / 256;
        x += mul_env(n, v->snap_env) / 8;
        return soft(x, ctl(v, 7));
    case DD_TRXF_CL:
        if (!v->double_done && v->age >= v->double_at) {
            v->double_done = 1;
            v->aux_env += ((ENV_ONE - v->aux_env) * ctl(v, 2)) >> 7;
        }
        a = dd_osc_sine(&v->osc[0], v->inc[0]);
        b = dd_osc_sine(&v->osc[1], v->inc[1]);
        x = mul_env(a, v->env) * 3 / 4;
        x += mul_env(b, v->aux_env) * (int32_t)(32u + ctl(v, 3)) / 512;
        x += mul_env(n, v->snap_env) * (int32_t)ctl(v, 5) / 2048;
        return dd_clamp_q15(x);
    default: return 0;
    }
}

void dd_trx_family_render(dd_trx_family_voice *v, const dd_trx_params *p,
                          int trigger, int32_t *out, uint32_t size)
{
    uint32_t i;
    int changed = !v->params_valid || trigger || v->last_params.level != p->level;
    for (i = 0; i < 8u && !changed; ++i)
        changed = v->last_params.control[i] != p->control[i];
    if (changed) {
        configure(v, p, trigger);
        for (i = 0; i < 8u; ++i) v->last_params.control[i] = p->control[i];
        v->last_params.level = p->level;
        v->params_valid = 1;
    }
    if (trigger) {
        v->age = 0;
        v->env = v->aux_env = v->snap_env = ENV_ONE;
        v->double_done = (v->kind != DD_TRXF_CL || ctl(v, 2) == 0u);
        for (i = 0; i < 6u; ++i) v->osc[i].phase = 0;
        for (i = 0; i < 8u; ++i) dd_onepole_init(&v->filter[i]);
        v->metal_prev = v->metal_next = 0;
        v->metal_phase = 0;
    }
    if (v->env == 0u && v->aux_env == 0u && v->snap_env == 0u &&
        (v->kind != DD_TRXF_CL || v->double_done)) {
        for (i = 0; i < size; ++i) out[i] = 0;
        return;
    }
    for (i = 0; i < size; ++i) {
        int32_t x;
        if (v->kind == DD_TRXF_CH || v->kind == DD_TRXF_OH) {
            if (v->metal_phase == 0u) {
                v->metal_next = v->env ? hat_half_sample(v) : 0;
                x = (v->metal_prev + v->metal_next) / 2;
                v->metal_phase = 1u;
            } else {
                x = v->metal_next;
                v->metal_prev = v->metal_next;
                v->metal_phase = 0u;
            }
        } else {
            x = sample(v);
        }
        x = dd_mul_q15(dd_clamp_q15(x), (int32_t)v->level);
        out[i] = x * 65536;
    }
}
