/* SPDX-License-Identifier: MIT
 * ==========================================================================
 * efm_bd.h -- EFM-BD bass drum, a portable floating-point FM voice.
 *
 * Readable, portable form of the algorithm: two sine operators, a phase-
 * modulation accumulator, a self-feedback operator, and one-pole envelope
 * coefficients. The voice is ~25 lines of math.
 *
 * The constants and control curves below were fitted to measurements of the
 * instrument's audio output: the control response was sampled across the
 * full knob travel and each coefficient was regressed against it. Overall
 * level tracks the instrument within about 2%; the spectral centroid within
 * about 0.3%. Every structural element of the voice is retained; the model
 * only drops numeric effects that fall below the noise floor in float.
 *
 * Accuracy target: ~95% perceptual, the engineering tolerance used for this
 * project.
 * ==========================================================================
 */
#ifndef EFM_BD_H
#define EFM_BD_H
#include <stdint.h>

/* Knob order as the host sends it (bank 0). */
enum { KNOB_PTCH, KNOB_DEC, KNOB_RAMP, KNOB_RDEC,
       KNOB_MOD, KNOB_MFRQ, KNOB_MDEC, KNOB_MFB, KNOB_COUNT };

/* Knobs converted to natural units (fractions of full scale, ~ -1..+1). */
typedef struct {
    double ptch;   /* pitch        : operator-1 frequency ratio        */
    double dec;    /* decay        : feedback envelope coefficient     */
    double ramp;   /* attack level : initial feedback seed (signed)    */
    double rdec;   /* rate/decay   : operator-1 envelope coefficient   */
    double mod;    /* mod          : operator-2 FM depth               */
    double mfrq;   /* mod frequency: carrier (operator-2) rate         */
    double mdec;   /* mod decay    : operator-2 envelope coefficient   */
    double mfb;    /* mod feedback : operator-1 phase-mod depth         */
} bd_controls;

/* Voice state. Only these six numbers are needed. */
typedef struct {
    double ph1;    /* operator-1 phase, 32768.0 == one sine cycle      */
    double ph2;    /* operator-2 phase, 32768.0 == one sine cycle      */
    double mod;    /* operator-1 phase-modulation, same index units    */
    double op1;    /* operator-1 envelope (fraction)                   */
    double op2;    /* operator-2 envelope (fraction)                   */
    double fb;     /* feedback envelope (fraction)                     */
} bd_state;

bd_controls bd_make_controls(const uint8_t knob[KNOB_COUNT]);

void   bd_init(bd_state *s);
void   bd_trigger(bd_state *s, const bd_controls *c);
double bd_sample(bd_state *s, const bd_controls *c);           /* one frame  */
void   bd_render(bd_state *s, const bd_controls *c, double *out, long n);

#endif /* EFM_BD_H */
