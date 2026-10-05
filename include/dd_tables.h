/* SPDX-License-Identifier: MIT */
#ifndef DD_TABLES_H
#define DD_TABLES_H

#include <stdint.h>

#define DD_SINE_SIZE_LOG2 9
#define DD_SINE_SIZE      (1u << DD_SINE_SIZE_LOG2)

extern const int16_t dd_sine_tab[DD_SINE_SIZE];

#define DD_EXP_SIZE 256
extern const int16_t dd_exp_decay_tab[DD_EXP_SIZE];

uint16_t dd_exp_decay_to_coeff(uint16_t decay_0_127);
int32_t  dd_decay_coeff_from_ms(uint32_t ms);

#endif
