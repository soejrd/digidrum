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

const dd_efm_tweaks dd_efm_default_tweaks[DD_EFM_MACHINE_COUNT] = {
    [DD_EFM_BD] = {
        .c_min_hz = 12, .c_hz_range = 390,
        .sweep_max_hz = 2400, .ramp_min_ms = 4, .ramp_span_ms = 500,
        .amp_min_ms = 12, .amp_span_ms = 1800,
        .mod_min_ms = 5, .mod_span_ms = 1200,
        .bd_mod_ratio_min_q8 = 20, .bd_mod_ratio_span_q8 = 2400,
        .bd_index_max_q8 = 2048, .bd_mod_attack_ms = 18,
        .fb_depth_mult = 75, .phase_offset_q2 = 2
    },
    [DD_EFM_SD] = {
        .c_min_hz = 90, .c_hz_range = 350, .m_min_hz = 300, .m_hz_range = 2800,
        .aux_min_ms = 5, .aux_span_ms = 1100,
        .amp_min_ms = 12, .amp_span_ms = 1800,
        .mod_min_ms = 5, .mod_span_ms = 1200,
        .depth_mult = 95, .fb_depth_fix = 1900, .noise_gain_mult = 210,
        .hp_min_hz = 30, .hp_hz_range = 2500
    },
    [DD_EFM_XT] = {
        .c_min_hz = 70, .c_hz_range = 430, .m_min_hz = 70, .m_hz_range = 2400,
        .sweep_max_hz = 450, .ramp_min_ms = 4, .ramp_span_ms = 500,
        .amp_min_ms = 12, .amp_span_ms = 1800,
        .mod_min_ms = 5, .mod_span_ms = 1200,
        .depth_mult = 105, .fb_depth_fix = 2800, .snap_gain_mult = 200,
        .hp_frac_num = 1, .hp_frac_den = 3
    },
    [DD_EFM_CP] = {
        .c_min_hz = 100, .c_hz_range = 1100, .m_min_hz = 100, .m_hz_range = 2900,
        .clap_max_count = 5, .clap_period = 480,
        .aux_min_ms = 3, .aux_span_ms = 50,
        .amp_min_ms = 12, .amp_span_ms = 1800,
        .mod_min_ms = 5, .mod_span_ms = 1200,
        .depth_mult = 120, .fb_depth_fix = 4800,
        .hp_min_hz = 80, .hp_hz_range = 3000
    },
    [DD_EFM_RS] = {
        .c_min_hz = 180, .c_hz_range = 920,
        .rim_mod_ratio = 2, .rim_mod_offset = 200,
        .c2_min_hz = 80, .c2_hz_range = 360,
        .m2_offset_hz = 850, .m2_hz_per_control = 10,
        .aux_min_ms = 10, .aux_span_ms = 850,
        .amp_min_ms = 12, .amp_span_ms = 1800, .mod_fixed_ms = 65,
        .depth_mult = 110, .fb_depth_fix = 2200,
        .noise_gain_mult = 258, .snap_gain_mult = 110,
        .hp_min_hz = 50, .hp_hz_range = 2600
    },
    [DD_EFM_CB] = {
        .c_min_hz = 190, .c_hz_range = 930, .m_min_hz = 250, .m_hz_range = 3000,
        .cb_ratio_percent = 148, .cb_aux_min_ms = 8, .cb_aux_divisor = 7,
        .amp_min_ms = 12, .amp_span_ms = 1800,
        .mod_min_ms = 5, .mod_span_ms = 1200,
        .depth_mult = 105, .fb_depth_mult = 55, .snap_gain_mult = 252
    },
    [DD_EFM_HH] = {
        .c_min_hz = 150, .c_hz_range = 700, .m_min_hz = 220, .m_hz_range = 2200,
        .ratio_0 = 1000, .ratio_1 = 1411, .ratio_2 = 1800, .ratio_3 = 2700,
        .amp_min_ms = 12, .amp_span_ms = 1100,
        .mod_min_ms = 5, .mod_span_ms = 1200,
        .depth_mult = 100, .fb_depth_mult = 50,
        .trem_depth_mult = 257, .trem_freq_min_hz = 2, .trem_freq_range = 68,
        .hp_fixed_hz = 450
    },
    [DD_EFM_CY] = {
        .c_min_hz = 150, .c_hz_range = 700, .m_min_hz = 220, .m_hz_range = 2200,
        .ratio_0 = 1000, .ratio_1 = 1411, .ratio_2 = 1800, .ratio_3 = 2700,
        .amp_min_ms = 80, .amp_span_ms = 3900,
        .mod_min_ms = 5, .mod_span_ms = 1200,
        .depth_mult = 100, .fb_depth_mult = 50,
        .hp_min_hz = 80, .hp_hz_range = 3300
    }
};

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

/* Listening matches supplied for Gearmulator knob -> old C knob position.
 * Piecewise segments preserve the measured anchors without assuming a
 * global power curve that misses the middle of the travel. */
static uint8_t bd_remap(uint8_t control, const uint8_t *input,
                        const uint8_t *output, uint32_t count)
{
    uint32_t i;
    for (i = 1; i < count; ++i) {
        if (control <= input[i]) {
            uint32_t width = input[i] - input[i - 1u];
            uint32_t rise = output[i] - output[i - 1u];
            return (uint8_t)(output[i - 1u] +
                ((uint32_t)(control - input[i - 1u]) * rise + width / 2u) / width);
        }
    }
    return output[count - 1u];
}

static uint8_t bd_decay_control(uint8_t control)
{
    static const uint8_t gearmulator[] = {0, 32, 64, 96, 127};
    static const uint8_t old_c[] = {0, 10, 25, 50, 127};
    return bd_remap(control, gearmulator, old_c, 5u);
}

static uint8_t bd_ramp_decay_control(uint8_t control)
{
    static const uint8_t gearmulator[] = {0, 40, 72, 93, 127};
    static const uint8_t old_c[] = {0, 22, 50, 88, 127};
    return bd_remap(control, gearmulator, old_c, 5u);
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
    const dd_efm_tweaks *t = &v->tweaks;
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
    v->depth = 0;
    v->fb_depth = 0;
    v->trem_depth = 0;
    switch (v->kind) {
    case DD_EFM_BD:
        v->bd_base_hz = t->c_min_hz + pitch * t->c_hz_range / 127u;
        v->bd_ratio_q8 = t->bd_mod_ratio_min_q8 +
            (uint32_t)v->control[5] * v->control[5] *
            t->bd_mod_ratio_span_q8 / (127u * 127u);
        v->bd_index_q8 = v->control[4] * t->bd_index_max_q8 / 127u;
        v->bd_mod_attack_samples = t->bd_mod_attack_ms * 48u;
        v->bd_mod_attack_step_q22 = v->bd_mod_attack_samples
            ? (32767u << 7) / v->bd_mod_attack_samples : 0u;
        v->sweep_inc = (int32_t)(v->control[2] * t->sweep_max_hz / 127u);
        dd_decay_env_set_coeff(&v->ramp, env_coeff(time_ms(
            bd_ramp_decay_control(v->control[3]), t->ramp_min_ms, t->ramp_span_ms)));
        v->fb_depth = v->control[7] * t->fb_depth_mult;
        break;
    case DD_EFM_SD:
        set_pair(v, 0, t->c_min_hz + pitch * t->c_hz_range / 127u,
                 t->m_min_hz + v->control[5] * t->m_hz_range / 127u);
        v->noise_gain = v->control[2] * t->noise_gain_mult;
        dd_decay_env_set_coeff(&v->aux, env_coeff(time_ms(v->control[3], t->aux_min_ms, t->aux_span_ms)));
        v->depth = v->control[4] * t->depth_mult;
        v->fb_depth = t->fb_depth_fix;
        hp = t->hp_min_hz + v->control[7] * t->hp_hz_range / 127u;
        break;
    case DD_EFM_XT:
        pitch = t->c_min_hz + pitch * t->c_hz_range / 127u;
        set_pair(v, 0, pitch, t->m_min_hz + v->control[5] * t->m_hz_range / 127u);
        v->sweep_inc = (int32_t)hz_inc(v->control[2] * t->sweep_max_hz / 127u);
        dd_decay_env_set_coeff(&v->ramp, env_coeff(time_ms(v->control[3], t->ramp_min_ms, t->ramp_span_ms)));
        v->depth = v->control[4] * t->depth_mult;
        v->fb_depth = t->fb_depth_fix;
        v->snap_gain = v->control[7] * t->snap_gain_mult;
        hp = pitch * t->hp_frac_num / (t->hp_frac_den ? t->hp_frac_den : 1u);
        break;
    case DD_EFM_CP:
        set_pair(v, 0, t->c_min_hz + pitch * t->c_hz_range / 127u,
                 t->m_min_hz + v->control[5] * t->m_hz_range / 127u);
        v->clap_count = 1u + v->control[2] * t->clap_max_count / 127u;
        v->clap_period = t->clap_period;
        dd_decay_env_set_coeff(&v->aux, env_coeff(time_ms(v->control[3], t->aux_min_ms, t->aux_span_ms)));
        v->depth = v->control[4] * t->depth_mult;
        v->fb_depth = t->fb_depth_fix;
        hp = t->hp_min_hz + v->control[7] * t->hp_hz_range / 127u;
        break;
    case DD_EFM_RS:
        pitch = t->c_min_hz + pitch * t->c_hz_range / 127u;
        set_pair(v, 0, pitch, pitch * t->rim_mod_ratio + t->rim_mod_offset);
        set_pair(v, 1, t->c2_min_hz + v->control[5] * t->c2_hz_range / 127u,
                 t->m2_offset_hz + v->control[5] * t->m2_hz_per_control);
        v->depth = v->control[2] * t->depth_mult;
        v->noise_gain = v->control[4] * t->noise_gain_mult;
        v->snap_gain = v->control[7] * t->snap_gain_mult;
        dd_decay_env_set_coeff(&v->aux, env_coeff(time_ms(v->control[6], t->aux_min_ms, t->aux_span_ms)));
        v->fb_depth = t->fb_depth_fix;
        hp = t->hp_min_hz + v->control[3] * t->hp_hz_range / 127u;
        break;
    case DD_EFM_CB:
        pitch = t->c_min_hz + pitch * t->c_hz_range / 127u;
        mf = t->m_min_hz + v->control[5] * t->m_hz_range / 127u;
        set_pair(v, 0, pitch, mf);
        set_pair(v, 1, pitch * t->cb_ratio_percent / 100u,
                 mf * t->cb_ratio_percent / 100u);
        v->snap_gain = v->control[2] * t->snap_gain_mult;
        v->fb_depth = v->control[3] * t->fb_depth_mult;
        v->depth = v->control[4] * t->depth_mult;
        dd_decay_env_set_coeff(&v->aux, env_coeff(t->cb_aux_min_ms + v->control[1] /
                                               (t->cb_aux_divisor ? t->cb_aux_divisor : 1u)));
        break;
    case DD_EFM_HH:
    case DD_EFM_CY: {
        const uint32_t ratio[4] = {t->ratio_0, t->ratio_1, t->ratio_2, t->ratio_3};
        uint32_t base = t->c_min_hz + pitch * t->c_hz_range / 127u;
        mf = t->m_min_hz + v->control[5] * t->m_hz_range / 127u;
        for (i = 0; i < 4u; ++i)
            set_pair(v, i, base * ratio[i] / 1000u,
                     mf * ratio[i] / 1000u);
        v->depth = v->control[4] * t->depth_mult;
        v->fb_depth = v->control[v->kind == DD_EFM_HH ? 7 : 2] * t->fb_depth_mult;
        if (v->kind == DD_EFM_HH) {
            v->trem_depth = v->control[2] * t->trem_depth_mult;
            v->trem_inc = (int32_t)hz_inc(t->trem_freq_min_hz +
                                         v->control[3] * t->trem_freq_range / 127u);
            hp = t->hp_fixed_hz;
        } else {
            hp = t->hp_min_hz + v->control[3] * t->hp_hz_range / 127u;
        }
        break;
    }
    }
    dd_decay_env_set_coeff(&v->amp, env_coeff(time_ms(
        v->kind == DD_EFM_BD ? bd_decay_control(v->control[1]) : v->control[1],
        t->amp_min_ms, t->amp_span_ms)));
    if (t->mod_fixed_ms)
        dd_decay_env_set_coeff(&v->mod, env_coeff(t->mod_fixed_ms));
    else
        dd_decay_env_set_coeff(&v->mod, env_coeff(time_ms(
            v->kind == DD_EFM_BD ? bd_decay_control(v->control[6]) : v->control[6],
            t->mod_min_ms, t->mod_span_ms)));
    v->hp_coeff = highpass_coeff(hp);
}

void dd_efm_init(dd_efm_voice *v, dd_efm_kind kind)
{
    uint32_t i;
    const unsigned char *defaults =
        (const unsigned char *)&dd_efm_default_tweaks[kind];
    volatile unsigned char *tweaks = (volatile unsigned char *)&v->tweaks;
    v->kind = kind;
    /* A struct assignment emits a libc memcpy on ColdFire; copy once at
     * voice initialization without adding a runtime import. */
    for (i = 0; i < sizeof(v->tweaks); ++i) tweaks[i] = defaults[i];
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
    v->bd_mod_attack_samples = v->bd_mod_attack_step_q22 = 0;
    v->bd_mod_decay_started = 0;
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

/* Figure 4.1 BD path: f(t) = fb(1 + Af Ef), fm(t) = f(t) * ratio,
 * fc(t) = f(t) * (1 + I Em modulator). Keep the frequency math in Hz and
 * Q8 so the sample loop needs only 32-bit multiplies on ColdFire. */
static int32_t bd_sample(dd_efm_voice *v, int32_t mod_env)
{
    int32_t base_hz = (int32_t)v->bd_base_hz +
        (v->sweep_inc * v->ramp.value >> 15);
    uint32_t mod_hz = (uint32_t)base_hz * v->bd_ratio_q8 >> 8;
    uint32_t mod_phase;
    int32_t mod_wave, excursion_hz, mod_wave_q8, carrier_hz;

    v->modulator[0].phase += hz_inc(mod_hz);
    mod_phase = v->modulator[0].phase +
        ((uint32_t)(v->feedback[0] * v->fb_depth) << 3);
    mod_wave = dd_sine_tab[mod_phase >> (32u - DD_SINE_SIZE_LOG2)];
    v->feedback[0] = mod_wave;

    excursion_hz = (base_hz * (int32_t)v->bd_index_q8) >> 8;
    mod_wave_q8 = dd_mul_q15(mod_wave, mod_env) >> 7;
    carrier_hz = base_hz + ((excursion_hz * mod_wave_q8) >> 8);
    if (carrier_hz > 11000) carrier_hz = 11000;
    if (carrier_hz < -11000) carrier_hz = -11000;
    v->carrier[0].phase += (uint32_t)(carrier_hz * 89478);
    return dd_sine_tab[v->carrier[0].phase >> (32u - DD_SINE_SIZE_LOG2)];
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
        v->bd_mod_decay_started = v->bd_mod_attack_samples == 0u;
        if (v->kind == DD_EFM_BD && v->bd_mod_attack_samples) {
            v->mod.value = 0;
            v->mod.active = 0;
        }
        dd_decay_env_trigger(&v->ramp);
        dd_decay_env_trigger(&v->aux);
        for (i = 0; i < 4u; ++i) {
            v->carrier[i].phase = (v->kind == DD_EFM_XT
                ? (uint32_t)v->control[7] * 0x40000000u / 127u : 0x40000000u)
                + v->tweaks.phase_offset_q2 * 0x40000000u;
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
            if (v->kind == DD_EFM_BD &&
                v->age >= v->bd_mod_attack_samples && !v->bd_mod_decay_started) {
                dd_decay_env_trigger(&v->mod);
                v->bd_mod_decay_started = 1;
            }
            dd_decay_env_step(&v->mod);
            dd_decay_env_step(&v->ramp);
            dd_decay_env_step(&v->aux);
            if (v->amp.value < 16 && v->aux.value < 16)
                v->active = 0;
        }
        if (!v->active) { out[n] = 0; continue; }
        a = v->amp.value;
        m = v->mod.value;
        if (v->kind == DD_EFM_BD && v->age < v->bd_mod_attack_samples)
            m = (int32_t)((v->age * v->bd_mod_attack_step_q22) >> 7);
        depth = dd_mul_q15(v->depth, m);
        if (v->kind == DD_EFM_XT)
            extra = (v->sweep_inc >> 15) * v->ramp.value;
        switch (v->kind) {
        case DD_EFM_BD:
            x = bd_sample(v, m);
            break;
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
