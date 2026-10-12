/* SPDX-License-Identifier: MIT
 * efm_bd.c -- EFM-BD as readable, portable C.
 *
 * The whole instrument is this one line, per output sample:
 *
 *      out = fb * sin(ph2)
 *
 * plus the six state updates inside bd_sample(). Everything else is turning
 * the 8 knobs into those numbers (bd_make_controls) and the trigger seed.
 *
 * Units used everywhere below:
 *   - envelopes op1/op2/fb : fractions, roughly 0..1
 *   - phases ph1/ph2/mod   : sine index units, 32768 == one full cycle
 *   - out                  : roughly -1..+1
 *
 * Signal flow (one sample):
 *
 *      ramp ----------> op1 -------> Y1          (operator 1 = "modulator")
 *                        |  *rdec   | *ptch
 *      fb --------------+           |
 *       ^                            v
 *       |                      ph1 += K1 * mfrq * Y1 ; s1 = sin(ph1)
 *       |                            |
 *     fb * dec                    mod = K1/8 * mfb * s1   (phase-mod op1)
 *       |                            |
 *       |                            v
 *      out <-- * fb <-- sin(ph2) <-- op2 <-- * mdec
 *                     ^              ^
 *                     |              |
 *              ph2 += K * (8*mod*(op2*s1) + Y1)      (op1 drives op2)
 *
 * with the two rate constants K = 32768 (per-unit) and K1 = 262144 = 8*K.
 */
#include "efm_bd.h"
#include <math.h>

#define TWO_PI 6.283185307179586476925286766559

bd_controls bd_make_controls(const uint8_t knob[8])
{
    bd_controls c;
    const double k = (double)knob[0];
    c.ptch = 0.000581 * pow(2.0, k / 32.0);
    c.dec  = 1.0 - 0.009469 * pow(2.0, -(double)knob[1] / 13.5);
    c.rdec = 1.0 - 0.009468 * pow(2.0, -(double)knob[3] / 12.6);
    c.mdec = 1.0 - 0.009474 * pow(2.0, -(double)knob[6] / 11.52);
    c.ramp = ((double)knob[2] - 16.0) / 128.0;
    c.mod  = (double)knob[4] / 128.0;
    c.mfb  = (double)knob[7] / 128.0;
    c.mfrq = (knob[5] < 1 ? 1.0 : (double)knob[5]) / 128.0;
    return c;
}

void bd_init(bd_state *s)
{
    s->ph1 = s->ph2 = s->mod = 0.0;
    s->op1 = s->op2 = s->fb  = 0.0;
}

void bd_trigger(bd_state *s, const bd_controls *c)
{
    s->op1 = c->ramp;                         /* operator 1 seeded from RAMP  */
    s->op2 = 1.0;                             /* full scale                   */
    s->fb  = 0.630957;                        /* feedback seed level          */
    s->ph1 = s->ph2 = 4000.0;                 /* start phase, index units     */
    s->mod = 0.0;
}

/* ------------------------------------------------------------------------- *
 *  One output sample. This is the entire algorithm.                         *
 * ------------------------------------------------------------------------- */
double bd_sample(bd_state *s, const bd_controls *c)
{
    /* --- operator 1: self-feedback "modulator" --- */
    const double op1_prev = s->op1;
    s->op1 = c->rdec * op1_prev;                       /* envelope decay     */
    const double y1 = c->ptch * (1.0 + 8.0 * op1_prev);

    /* --- read operator-1 sine, phase-modulated by mod --- */
    const int    i1 = ((int)floor(s->ph1 + s->mod)) & 0x7fff;
    const double s1 = sin(TWO_PI * (double)i1 / 32768.0);

    /* --- advance operator 1; set its phase-modulation for next sample --- */
    s->ph1 += 262144.0 * c->mfrq * y1;
    s->mod  = 32768.0 * c->mfb * s1;

    /* --- operator 2: main carrier, driven by operator 1 --- */
    const double op2_prev = s->op2;
    s->op2 = c->mdec * op2_prev;                       /* envelope decay     */
    const double c0  = op2_prev * s1;                  /* op1 output         */
    const double x1c = c0 * y1;

    const int    i2 = ((int)floor(s->ph2)) & 0x7fff;
    const double s2 = sin(TWO_PI * (double)i2 / 32768.0);
    s->ph2 += 32768.0 * (8.0 * c->mod * x1c + y1);

    /* --- output = feedback envelope * operator 2 --- */
    const double out = s->fb * s2;
    s->fb = c->dec * s->fb;
    return out;
}

void bd_render(bd_state *s, const bd_controls *c, double *out, long n)
{
    for (long i = 0; i < n; ++i) out[i] = bd_sample(s, c);
}

