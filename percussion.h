/* SPDX-License-Identifier: MIT */
#ifndef DIGIPERC_PERCUSSION_H
#define DIGIPERC_PERCUSSION_H

#include <stdint.h>

#define DP_BLOCK_SIZE 32

/* All control values are unsigned Q15 in the range 0..32767. frequency is the
 * state-variable filter coefficient 2*sin(pi*Hz/48000), also Q15. */
struct dp_params {
    uint16_t frequency;
    uint16_t tone;
    uint16_t decay;
    uint16_t attack_fm;
    uint16_t self_fm;
    uint16_t drive;
    uint16_t accent;
    uint16_t level;
};

struct dp_voice {
    int32_t low;
    int32_t band;
    int32_t pulse;
    int32_t pulse_lp;
    int32_t fm_env;
    int32_t fm_lp;
    int32_t tone_lp;
    uint16_t pulse_samples;
    uint16_t fm_samples;
    uint16_t quiet_samples;
    uint8_t active;
};

void dp_voice_init(struct dp_voice *voice);
void dp_voice_render(
    struct dp_voice *voice,
    const struct dp_params *params,
    int trigger,
    int32_t *output,
    uint32_t size);

#endif
