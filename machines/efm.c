/* SPDX-License-Identifier: MIT
 * EFM percussion voices. Signal paths follow Larsson's FM drum thesis and
 * the local md-drum-synth models; control curves are listening prototypes.
 * 48 kHz, fixed point, no allocation, transcendental calls or per-sample division.
 */
#include "../include/dd_efm.h"
#include "../include/dd_fixed.h"
#include "../include/dd_tables.h"

/* Controls use the Machinedrum order supplied in the manual. */
const uint8_t dd_efm_defaults_u7[DD_EFM_MACHINE_COUNT][8] = {
    {45, 65, 58, 48, 42, 35, 45, 25}, /* BD */
    {55, 55, 55, 48, 55, 60, 42, 40}, /* SD */
    {50, 65, 50, 48, 45, 40, 50, 36}, /* XT */
    {64, 52, 64, 30, 70, 55, 42, 55}, /* CP */
    {65, 40, 55, 60, 40, 50, 55, 45}, /* RS */
    {55, 60, 65, 45, 50, 65, 45, 0},  /* CB */
    {65, 42, 35, 45, 50, 60, 40, 40}, /* HH */
    {55, 78, 45, 55, 60, 65, 52, 0}  /* CY */
};
const uint8_t dd_efm_control_counts[DD_EFM_MACHINE_COUNT] =
    {8, 8, 8, 8, 8, 7, 8, 7};

static uint8_t u7(uint16_t q15)
{
    return (uint8_t)(((uint32_t)q15 * 127u + 16383u) / 32767u);
}

static uint32_t hz_inc(uint32_t hz)
{
    if (hz > 11000u) hz = 11000u;
    return hz * 89478u; /* round(2^32 / 48000) */
}

/* Step each shared Q15 envelope once per eight samples. This covers long
 * cymbal decays without adding wide multiplies to the sample loop. */
static int32_t env_coeff(uint32_t ms)
{
    uint32_t loss = 5461u / (ms ? ms : 1u);
    if (loss < 1u) loss = 1u;
    if (loss > 32767u) loss = 32767u;
    return (int32_t)(32768u - loss);
}

static uint32_t time_ms(uint8_t control, uint32_t min_ms, uint32_t span_ms)
{
    uint32_t c = control;
    return min_ms + (c * c * span_ms) / (127u * 127u);
}

static int32_t highpass_coeff(uint32_t hz)
{
    return (int32_t)(1572816000u / (48000u + 6u * hz));
}

static void set_pair(dd_efm_voice *v, uint32_t pair, uint32_t car_hz,
                     uint32_t mod_hz)
{
    v->carrier_inc[pair] = hz_inc(car_hz);
    v->mod_inc[pair] = hz_inc(mod_hz);
}

static void update(dd_efm_voice *v, const dd_trx_params *p)
{
    uint32_t i, pitch, mf, hp = 0;
    struct dd_params cache_params;
    uint16_t changed;
    for (i = 0; i < 7u; ++i) cache_params.p[i] = p->control[i];
    cache_params.level = p->control[7];
    changed = dd_param_cache_update(&v->cache, &cache_params);
    if (!changed && v->level == p->level) return;
    for (i = 0; i < 8u; ++i) v->control[i] = u7(p->control[i]);
    v->level = p->level;
    pitch = v->control[0];
    v->noise_gain = 0;
    v->snap_gain = 0;
    v->sweep_inc = 0;
    v->fb_depth = 0;
    v->trem_depth = 0;
    switch (v->kind) {
    case DD_EFM_BD:
        set_pair(v, 0, 25u + pitch * 95u / 127u,
                 40u + v->control[5] * 1600u / 127u);
        v->sweep_inc = (int32_t)hz_inc(v->control[2] * 600u / 127u);
        dd_decay_env_set_coeff(&v->ramp, env_coeff(time_ms(v->control[3], 4, 500)));
        v->depth = v->control[4] * 105;
        v->fb_depth = v->control[7] * 40;
        break;
    case DD_EFM_SD:
        set_pair(v, 0, 90u + pitch * 350u / 127u,
                 300u + v->control[5] * 2800u / 127u);
        v->noise_gain = v->control[2] * 210u;
        dd_decay_env_set_coeff(&v->aux, env_coeff(time_ms(v->control[3], 5, 1100)));
        v->depth = v->control[4] * 95;
        v->fb_depth = 1900; /* fixed noisy modulator, per EFM paper */
        hp = 30u + v->control[7] * 2500u / 127u;
        break;
    case DD_EFM_XT:
        pitch = 70u + pitch * 430u / 127u;
        set_pair(v, 0, pitch, 70u + v->control[5] * 2400u / 127u);
        v->sweep_inc = (int32_t)hz_inc(v->control[2] * 450u / 127u);
        dd_decay_env_set_coeff(&v->ramp, env_coeff(time_ms(v->control[3], 4, 500)));
        v->depth = v->control[4] * 105;
        v->fb_depth = 2800;
        v->snap_gain = v->control[7] * 200;
        hp = pitch / 3u;
        break;
    case DD_EFM_CP:
        set_pair(v, 0, 100u + pitch * 1100u / 127u,
                 100u + v->control[5] * 2900u / 127u);
        v->clap_count = 1u + v->control[2] * 5u / 127u;
        v->clap_period = 480u; /* 10 ms between preclaps */
        dd_decay_env_set_coeff(&v->aux, env_coeff(time_ms(v->control[3], 3, 50)));
        v->depth = v->control[4] * 120;
        v->fb_depth = 4800;
        hp = 80u + v->control[7] * 3000u / 127u;
        break;
    case DD_EFM_RS:
        pitch = 180u + pitch * 920u / 127u;
        set_pair(v, 0, pitch, pitch * 2u + 200u);
        set_pair(v, 1, 80u + v->control[5] * 360u / 127u,
                 850u + v->control[5] * 10u);
        v->depth = v->control[2] * 110;
        v->noise_gain = v->control[4] * 258u; /* snare body mix */
        v->snap_gain = v->control[7] * 110; /* snare modulation */
        dd_decay_env_set_coeff(&v->aux, env_coeff(time_ms(v->control[6], 10, 850)));
        v->fb_depth = 2200;
        hp = 50u + v->control[3] * 2600u / 127u;
        break;
    case DD_EFM_CB:
        pitch = 190u + pitch * 930u / 127u;
        mf = 250u + v->control[5] * 3000u / 127u;
        set_pair(v, 0, pitch, mf);
        set_pair(v, 1, pitch * 148u / 100u, mf * 148u / 100u);
        v->snap_gain = v->control[2] * 252u;
        v->fb_depth = v->control[3] * 55;
        v->depth = v->control[4] * 105;
        dd_decay_env_set_coeff(&v->aux, env_coeff(8u + v->control[1] / 7u));
        break;
    case DD_EFM_HH:
    case DD_EFM_CY: {
        static const uint16_t ratio[4] = {1000, 1411, 1800, 2700};
        uint32_t base = 150u + pitch * 700u / 127u;
        mf = 220u + v->control[5] * 2200u / 127u;
        for (i = 0; i < 4u; ++i)
            set_pair(v, i, base * ratio[i] / 1000u,
                     mf * ratio[i] / 1000u);
        v->depth = v->control[4] * 100;
        v->fb_depth = v->control[v->kind == DD_EFM_HH ? 7 : 2] * 50;
        if (v->kind == DD_EFM_HH) {
            v->trem_depth = v->control[2] * 257u;
            v->trem_inc = (int32_t)hz_inc(2u + v->control[3] * 68u / 127u);
            hp = 450u; /* fixed bright hi-hat */
        } else {
            hp = 80u + v->control[3] * 3300u / 127u;
        }
        break;
    }
    }
    if (v->kind == DD_EFM_HH)
        dd_decay_env_set_coeff(&v->amp, env_coeff(time_ms(v->control[1], 12, 1100)));
    else if (v->kind == DD_EFM_CY)
        dd_decay_env_set_coeff(&v->amp, env_coeff(time_ms(v->control[1], 80, 3900)));
    else
        dd_decay_env_set_coeff(&v->amp, env_coeff(time_ms(v->control[1], 12, 1800)));
    if (v->kind == DD_EFM_CB)
        dd_decay_env_set_coeff(&v->mod, env_coeff(time_ms(v->control[6], 5, 1200)));
    else if (v->kind == DD_EFM_RS)
        dd_decay_env_set_coeff(&v->mod, env_coeff(65));
    else
        dd_decay_env_set_coeff(&v->mod, env_coeff(time_ms(v->control[6], 5, 1200)));
    v->hp_coeff = highpass_coeff(hp);
}

void dd_efm_init(dd_efm_voice *v, dd_efm_kind kind)
{
    uint32_t i;
    v->kind = kind;
    for (i = 0; i < 4u; ++i) {
        dd_osc_init(&v->carrier[i]);
        dd_osc_init(&v->modulator[i]);
        v->feedback[i] = 0;
    }
    dd_osc_init(&v->tremolo);
    dd_decay_env_init(&v->amp);
    dd_decay_env_init(&v->mod);
    dd_decay_env_init(&v->ramp);
    dd_decay_env_init(&v->aux);
    dd_onepole_init(&v->hp[0]);
    dd_onepole_init(&v->hp[1]);
    dd_noise_init(&v->noise, 0x7a3d91e5u + (uint32_t)kind * 101u);
    dd_param_cache_init(&v->cache);
    v->age = v->clap_time = v->clap_period = 0;
    v->clap_count = v->clap_stage = v->active = 0;
    v->level = 32767;
}

/* Phase modulation keeps one oscillator update per operator and avoids a
 * variable-frequency carrier increment. Depth is in phase-table units. */
static int32_t pair(dd_efm_voice *v, uint32_t i, int32_t depth,
                    uint32_t car_extra)
{
    uint32_t phase, mod_phase;
    int32_t m, c;
    int carrier_feedback = v->kind == DD_EFM_CB ||
        v->kind == DD_EFM_HH || v->kind == DD_EFM_CY;
    v->modulator[i].phase += v->mod_inc[i];
    mod_phase = v->modulator[i].phase;
    if (!carrier_feedback)
        mod_phase += (uint32_t)(v->feedback[i] * v->fb_depth) << 3;
    m = dd_sine_tab[mod_phase >> (32u - DD_SINE_SIZE_LOG2)];
    v->carrier[i].phase += v->carrier_inc[i] + car_extra;
    phase = v->carrier[i].phase + ((uint32_t)(m * depth) << 3);
    if (carrier_feedback)
        phase += (uint32_t)(v->feedback[i] * v->fb_depth) << 3;
    c = dd_sine_tab[phase >> (32u - DD_SINE_SIZE_LOG2)];
    v->feedback[i] = carrier_feedback ? c : m;
    return c;
}

void dd_efm_render(dd_efm_voice *v, const dd_trx_params *p,
                   int trigger, int32_t *out, uint32_t size)
{
    uint32_t n, i;
    update(v, p);
    if (trigger) {
        v->age = v->clap_time = 0;
        v->clap_stage = 0;
        v->active = 1;
        dd_decay_env_trigger(&v->amp);
        dd_decay_env_trigger(&v->mod);
        dd_decay_env_trigger(&v->ramp);
        dd_decay_env_trigger(&v->aux);
        for (i = 0; i < 4u; ++i) {
            v->carrier[i].phase = v->kind == DD_EFM_XT
                ? (uint32_t)v->control[7] * 0x40000000u / 127u : 0x40000000u;
            v->modulator[i].phase = 0;
            v->feedback[i] = 0;
        }
        v->tremolo.phase = 0;
        dd_onepole_init(&v->hp[0]);
        dd_onepole_init(&v->hp[1]);
    }
    for (n = 0; n < size; ++n) {
        int32_t a, m, x = 0, depth, extra = 0;
        if (!v->active) { out[n] = 0; continue; }
        if ((v->age & 7u) == 0u) {
            dd_decay_env_step(&v->amp);
            dd_decay_env_step(&v->mod);
            dd_decay_env_step(&v->ramp);
            dd_decay_env_step(&v->aux);
            if (v->amp.value < 16 && v->aux.value < 16)
                v->active = 0;
        }
        if (!v->active) { out[n] = 0; continue; }
        a = v->amp.value;
        m = v->mod.value;
        depth = dd_mul_q15(v->depth, m);
        if (v->kind == DD_EFM_BD || v->kind == DD_EFM_XT)
            extra = (v->sweep_inc >> 15) * v->ramp.value;
        switch (v->kind) {
        case DD_EFM_BD:
        case DD_EFM_XT:
            x = pair(v, 0, depth, (uint32_t)extra);
            if (v->kind == DD_EFM_XT && v->age < 12u)
                x += dd_mul_q15(dd_noise_q15(&v->noise),
                                v->snap_gain * (12 - (int32_t)v->age) / 12);
            break;
        case DD_EFM_SD:
            x = pair(v, 0, depth, 0) >> 1;
            x += dd_mul_q15(dd_noise_q15(&v->noise),
                            dd_mul_q15(v->noise_gain, v->aux.value));
            break;
        case DD_EFM_CP:
            if (v->clap_stage < v->clap_count &&
                ++v->clap_time >= v->clap_period) {
                v->clap_time = 0;
                ++v->clap_stage;
                dd_decay_env_trigger(&v->aux);
                if (v->clap_stage == v->clap_count)
                    dd_decay_env_trigger(&v->amp);
            }
            x = pair(v, 0, depth, 0);
            x = dd_mul_q15(x, v->clap_stage < v->clap_count
                           ? v->aux.value : a);
            /* Clap envelope has already been applied. */
            a = 32767;
            break;
        case DD_EFM_RS: {
            int32_t rim = pair(v, 0, depth, 0);
            int32_t body = pair(v, 1,
                dd_mul_q15(v->snap_gain, m), 0);
            x = dd_mul_q15(rim, 32767 - v->noise_gain) +
                dd_mul_q15(dd_mul_q15(body, v->noise_gain), v->aux.value);
            break;
        }
        case DD_EFM_CB: {
            int32_t first = pair(v, 0, depth, 0);
            int32_t second = pair(v, 1, depth, 0);
            int32_t snap = dd_mul_q15(v->snap_gain, v->aux.value);
            x = dd_mul_q15((first + second) >> 1, 32767 - snap) +
                dd_mul_q15(first, snap);
            break;
        }
        case DD_EFM_HH:
        case DD_EFM_CY:
            for (i = 0; i < 4u; ++i) x += pair(v, i, depth, 0) >> 2;
            if (v->kind == DD_EFM_HH) {
                int32_t trem = dd_osc_sine(&v->tremolo, (uint32_t)v->trem_inc);
                x = dd_mul_q15(x, 32767 - (v->trem_depth >> 1) +
                               (dd_mul_q15(trem, v->trem_depth) >> 1));
            }
            break;
        }
        x = dd_mul_q15(dd_clamp_q15(x), a);
        if (v->kind == DD_EFM_SD || v->kind == DD_EFM_CP ||
            v->kind == DD_EFM_RS || v->kind == DD_EFM_XT ||
            v->kind == DD_EFM_HH || v->kind == DD_EFM_CY) {
            x = dd_onepole_hp_fast(&v->hp[0], x, v->hp_coeff);
            if (v->kind == DD_EFM_CY)
                x = dd_onepole_hp_fast(&v->hp[1], x, v->hp_coeff);
        }
        x = dd_mul_q15(dd_clamp_q15(x), v->level);
        out[n] = x * 65536;
        ++v->age;
    }
}
