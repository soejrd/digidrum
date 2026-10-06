/* SPDX-License-Identifier: MIT
 *
 * Fixed-point percussion voice adapted from the signal topology of Mutable
 * Instruments Plaits' analog_bass_drum.h.
 * Copyright (c) 2016 Emilie Gillet (original Plaits implementation).
 * Copyright (c) 2026 digiperc contributors (fixed-point port).
 * See THIRD_PARTY.md for the complete MIT notice and provenance.
 */
#include "percussion.h"

#define Q15_ONE 32767
#define PULSE_TIME 48       /* 1 ms at 48 kHz */
#define FM_TIME 288         /* 6 ms at 48 kHz */

static int32_t dp_abs(int32_t x)
{
    return x < 0 ? -x : x;
}

static int32_t dp_clamp(int32_t x, int32_t lo, int32_t hi)
{
    if (x < lo)
        return lo;
    if (x > hi)
        return hi;
    return x;
}

/* Inputs are kept below +/-65535, and coefficients below 32768, so the
 * product fits a signed 32-bit word. This deliberately avoids 64-bit helpers
 * on the ColdFire target. */
static int32_t dp_mul_q15(int32_t a, int32_t b)
{
    return (a * b) >> 15;
}

static int32_t dp_soft_clip(int32_t x, uint16_t drive)
{
    int32_t gain = Q15_ONE + ((int32_t)drive >> 1);
    int32_t y = dp_mul_q15(x, gain);
    int32_t a = dp_abs(y);

    /* A bounded rational-like curve without division. The quadratic knee
     * starts at half scale; the final clamp is intentional saturation. */
    if (a > 16384) {
        int32_t over = a - 16384;
        a = 16384 + over - ((over * over) >> 15);
        if (a > Q15_ONE)
            a = Q15_ONE;
        y = y < 0 ? -a : a;
    }
    return y;
}

void dp_voice_init(struct dp_voice *v)
{
    v->low = 0;
    v->band = 0;
    v->pulse = 0;
    v->pulse_lp = 0;
    v->fm_env = 0;
    v->fm_lp = 0;
    v->tone_lp = 0;
    v->pulse_samples = 0;
    v->fm_samples = 0;
    v->quiet_samples = 0;
    v->active = 0;
}

void dp_voice_render(
    struct dp_voice *v,
    const struct dp_params *p,
    int trigger,
    int32_t *out,
    uint32_t size)
{
    uint32_t i;
    int32_t damping = 1250 - ((int32_t)p->decay * 1080 >> 15);
    int32_t tone_f = 384 + ((int32_t)p->tone * 12000 >> 15);

    if (trigger) {
        v->pulse_samples = PULSE_TIME;
        v->fm_samples = FM_TIME;
        v->pulse = 18000 + ((int32_t)p->accent * 12000 >> 15);
        v->fm_env = Q15_ONE;
        v->quiet_samples = 0;
        v->active = 1;
        /* As in Plaits, a new strike clears the low-pass state but leaves the
         * resonator alive, so rapid retriggers interact with the old body. */
        v->tone_lp = 0;
    }

    for (i = 0; i < size; ++i) {
        int32_t pulse;
        int32_t excitation;
        int32_t attack;
        int32_t self;
        int32_t f;
        int32_t high;
        int32_t body;
        int32_t mixed;
        int32_t y;

        if (!v->active) {
            out[i] = 0;
            continue;
        }

        if (v->pulse_samples) {
            --v->pulse_samples;
            pulse = v->pulse_samples ? v->pulse : v->pulse - 8192;
        } else {
            v->pulse -= v->pulse >> 3;
            pulse = v->pulse;
        }

        /* Plaits' trigger path removes DC before its asymmetric diode. */
        v->pulse_lp += dp_mul_q15(pulse - v->pulse_lp, 6827);
        excitation = pulse - v->pulse_lp + (pulse >> 5);
        if (excitation < 0)
            excitation >>= 1;

        if (v->fm_samples) {
            --v->fm_samples;
        } else {
            v->fm_env -= v->fm_env >> 5;
        }
        v->fm_lp += dp_mul_q15(v->fm_env - v->fm_lp, 6827);

        attack = dp_mul_q15(v->fm_lp, p->attack_fm);
        self = dp_mul_q15(dp_abs(v->low), p->self_fm);
        f = p->frequency + ((int32_t)p->frequency * (attack + (self >> 2)) >> 14);
        f = dp_clamp(f, 32, 6000);

        /* Chamberlin state-variable resonator. The low coefficient range used
         * by a bass drum keeps this stable while producing the bridged-T-like
         * ringing body of the source algorithm. */
        high = excitation - v->low - dp_mul_q15(v->band, damping);
        v->band = dp_clamp(v->band + dp_mul_q15(high, f), -65535, 65535);
        v->low = dp_clamp(v->low + dp_mul_q15(v->band, f), -65535, 65535);
        body = v->band;

        mixed = body + dp_mul_q15(excitation, 2048 + ((int32_t)p->tone >> 3));
        v->tone_lp += dp_mul_q15(mixed - v->tone_lp, tone_f);
        /* The resonator intentionally runs with ample headroom. Restore a
         * useful voice level before the saturator and the user's LEV. */
        y = dp_soft_clip(dp_clamp(v->tone_lp * 4, -32767, 32767), p->drive);
        y = dp_mul_q15(y, p->level);
        y = dp_clamp(y, -32767, 32767);
        out[i] = y * 65536;

        if (dp_abs(v->low) + dp_abs(v->band) + dp_abs(v->tone_lp) < 20 &&
            !v->pulse_samples && !v->fm_samples) {
            if (++v->quiet_samples > 1024) {
                dp_voice_init(v);
            }
        } else {
            v->quiet_samples = 0;
        }
    }
}
