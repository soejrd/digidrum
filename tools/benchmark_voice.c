/* SPDX-License-Identifier: MIT
 * Synthetic benchmark voice exercising the full primitive library.
 *
 * Topology:
 *   pitch-swept sine -> resonator body
 *   + noise burst -> one-pole filter
 *   -> sum -> bit quantizer -> soft clip -> amplitude decay
 *
 * Internal rate: HALF (host / 2), with zero-order-hold output.
 *
 * This is not a musically finalized instrument. It exists to prove the
 * primitives compose, cross-compile and render deterministically.
 */
#include "benchmark_voice.h"
#include "dd_fixed.h"
#include "dd_tables.h"

static void bv_trigger(struct benchmark_voice *v, const struct dd_params *p)
{
    dd_decay_env_set_coeff(&v->amp_env, dd_exp_decay_to_coeff(p->p[DD_DECAY] >> 2));
    dd_decay_env_trigger(&v->amp_env);

    dd_decay_env_set_coeff(&v->noise_env, 32700);
    dd_decay_env_trigger(&v->noise_env);

    v->sweep.base = 0;
    v->sweep.target = (p->p[DD_P4] >> 1);
    v->sweep.value = v->sweep.target;
    v->sweep.decay_coeff = 32600;
    v->sweep.active = 1;

    dd_downsampler_init(&v->ds);
    v->zoh_prev = 0;
    v->zoh_next = 0;
}

static int32_t bv_render_half(struct benchmark_voice *v,
                              const struct dd_params *p)
{
    uint32_t body_inc = (uint32_t)(p->p[DD_PITCH] >> 2) + 128u;
    int32_t noise_amp = dd_mul_q15(p->p[1], v->noise_env.value);
    int32_t bits = (p->p[1] >> 1) + 6;
    int32_t drive = p->p[DD_DRIVE];

    int32_t body = dd_osc_sine_interp(&v->body_osc, body_inc);
    body = dd_mul_q15(body, 32767 - (v->sweep.value >> 2));
    body = dd_mul_q15(body, 16384);

    int32_t noise = dd_mul_q15(noise_amp, 24576);
    noise = dd_onepole_lp(&v->noise_lp, noise, 32000);

    int32_t mixed = body + noise;
    mixed = dd_bit_quantize(mixed, bits);
    mixed = dd_soft_clip(mixed, drive);

    v->sweep.value = dd_mul_q15(v->sweep.value, v->sweep.decay_coeff);
    if (v->sweep.value >= -2 && v->sweep.value <= 2) {
        v->sweep.value = 0;
        v->sweep.active = 0;
    }

    v->noise_env.value = dd_mul_q15(v->noise_env.value, 32200);
    if (v->noise_env.value < 16) {
        v->noise_env.value = 0;
        v->noise_env.active = 0;
    }

    if (!v->amp_env.active)
        return 0;

    int32_t amp = dd_decay_env_step(&v->amp_env);

    mixed = dd_mul_q15(mixed, amp);
    mixed = dd_mul_q15(mixed, p->level);
    return dd_clamp_q15(mixed);
}

void benchmark_voice_init(struct benchmark_voice *v)
{
    dd_osc_init(&v->body_osc);
    dd_noise_init(&v->rng, 0x12345678u);
    dd_decay_env_init(&v->amp_env);
    dd_decay_env_init(&v->noise_env);
    dd_pitch_sweep_init(&v->sweep);
    dd_onepole_init(&v->noise_lp);
    dd_downsampler_init(&v->ds);
    v->zoh_prev = 0;
    v->zoh_next = 0;
}

void benchmark_voice_render(struct benchmark_voice *v,
                            const struct dd_params *p,
                            int trigger,
                            int32_t *out,
                            uint32_t size)
{
    uint32_t i;
    int32_t half_rate_sample = 0;

    if (trigger)
        bv_trigger(v, p);

    for (i = 0; i < size; ++i) {
        if (dd_downsampler_should_process(&v->ds)) {
            half_rate_sample = bv_render_half(v, p);
            v->zoh_prev = v->zoh_next;
            v->zoh_next = half_rate_sample;
        }
        out[i] = v->zoh_prev * 65536;
    }
}
