/* SPDX-License-Identifier: MIT */
#ifndef DD_MACHINE_H
#define DD_MACHINE_H

#include <stdint.h>

#define DD_PARAMS_COUNT 7
#define DD_BLOCK_SIZE   32

struct dd_params {
    uint16_t p[DD_PARAMS_COUNT];
    uint16_t level;
};

#define DD_PITCH  0
#define DD_P2     1
#define DD_P3     2
#define DD_P4     3
#define DD_P5     4
#define DD_DECAY  5
#define DD_DRIVE  6

typedef enum {
    DD_RATE_FULL    = 1,
    DD_RATE_HALF    = 2,
    DD_RATE_QUARTER = 4,
} dd_internal_rate;

struct dd_machine_desc {
    uint8_t id;
    const char *name;
    const char *short_name;
    uint8_t internal_rate_div;
    uint16_t state_size;
    void (*init)(void *voice);
    void (*render)(void *voice,
                   const struct dd_params *p,
                   int trigger,
                   int32_t *out,
                   uint32_t size);
};

#endif
