/* SPDX-License-Identifier: MIT
 * Synthetic benchmark voice exercising the full primitive library.
 *
 * Topology:
 *   pitch-swept sine -> resonator body
 *   + noise burst -> one-pole filter
 *   -> sum -> bit quantizer -> soft clip -> amplitude decay
 *
 * Internal rate: HALF (host / 2), with zero-order-hold output.
 */
#ifndef BENCHMARK_VOICE_H
#define BENCHMARK_VOICE_H

#include <stdint.h>
#include "dd_machine.h"
#include "dd_osc.h"
#include "dd_envelope.h"
#include "dd_noise.h"
#include "dd_filter.h"
#include "dd_resonator.h"
#include "dd_rate.h"

struct benchmark_voice {
    dd_osc body_osc;
    dd_noise rng;
    dd_decay_env amp_env;
    dd_decay_env noise_env;
    dd_pitch_sweep_env sweep;
    dd_onepole noise_lp;
    dd_downsampler ds;
    int32_t zoh_prev;
    int32_t zoh_next;
};

void benchmark_voice_init(struct benchmark_voice *v);
void benchmark_voice_render(struct benchmark_voice *v,
                            const struct dd_params *p,
                            int trigger,
                            int32_t *out,
                            uint32_t size);

#endif
