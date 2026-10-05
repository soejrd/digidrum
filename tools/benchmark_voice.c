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
    dd_decay_env_set_coeff(&v->amp_env,
                           dd_exp_decay_to_coeff((uint16_t)dd_clamp(p->p[DD_DECAY] >> 8, 0, 127)));
    dd_decay_env_trigger(&v->amp_env);

    dd_decay_env_set_coeff(&v->noise_env, 32200);
    dd_decay_env_trigger(&v->noise_env);

    dd_pitch_sweep_trigger(&v->sweep, 0, p->p[DD_P4] >> 1, 32600);
    dd_resonator_init(&v->body_res);
    dd_resonator_set(&v->body_res, 214, 16384);

    dd_downsampler_init(&v->ds);
    v->zoh_prev = 0;
    v->zoh_next = 0;
}

static int32_t bv_render_half(struct benchmark_voice *v,
                              const struct dd_params *p)
{
    uint32_t body_inc = 4000000u + (uint32_t)p->p[DD_PITCH] * 8000u
                        + (uint32_t)v->sweep.value * 256u;
    int32_t noise_amp = dd_mul_q15(p->p[1], v->noise_env.value);
    int32_t bits = 4 + (p->p[1] >> 12);
    int32_t drive = p->p[DD_DRIVE];

    int32_t body = dd_osc_sine_interp(&v->body_osc, body_inc);
    body = dd_resonator_lp(&v->body_res, body);
    body = dd_mul_q15(body, 16384);

    int32_t noise = dd_mul_q15(dd_noise_q15(&v->rng), noise_amp);
    noise = dd_mul_q15(noise, 24576);
    noise = dd_onepole_lp(&v->noise_lp, noise, 32000);

    int32_t mixed = body + noise;
    mixed = dd_bit_quantize(mixed, bits);
    mixed = dd_soft_clip(mixed, drive);

    dd_pitch_sweep_step(&v->sweep);

    dd_decay_env_step(&v->noise_env);

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
    dd_resonator_init(&v->body_res);
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
