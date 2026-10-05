/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <stdint.h>
#include <stdio.h>

#include "../percussion.h"

static void u16le(FILE *f, uint16_t x)
{
    fputc(x & 0xff, f);
    fputc(x >> 8, f);
}

static void u32le(FILE *f, uint32_t x)
{
    u16le(f, x & 0xffff);
    u16le(f, x >> 16);
}

int main(void)
{
    struct dp_voice voice;
    struct dp_params p = {214, 19000, 23500, 27000, 10000, 9000, 28000, 29000};
    int32_t block[DP_BLOCK_SIZE];
    FILE *f = fopen("out/pulse-bd.wav", "wb");
    uint32_t remaining = 48000;
    int trigger = 1;

    if (!f)
        return 1;
    fwrite("RIFF", 1, 4, f);
    u32le(f, 36 + 48000 * 2);
    fwrite("WAVEfmt ", 1, 8, f);
    u32le(f, 16);
    u16le(f, 1);
    u16le(f, 1);
    u32le(f, 48000);
    u32le(f, 96000);
    u16le(f, 2);
    u16le(f, 16);
    fwrite("data", 1, 4, f);
    u32le(f, 48000 * 2);

    dp_voice_init(&voice);
    while (remaining) {
        uint32_t n = remaining > DP_BLOCK_SIZE ? DP_BLOCK_SIZE : remaining;
        uint32_t i;
        dp_voice_render(&voice, &p, trigger, block, n);
        trigger = 0;
        for (i = 0; i < n; ++i)
            u16le(f, (uint16_t)(block[i] >> 16));
        remaining -= n;
    }
    fclose(f);
    return 0;
}
