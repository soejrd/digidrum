/* SPDX-License-Identifier: MIT */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "benchmark_voice.h"
#include "dd_machine.h"

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
    struct benchmark_voice voice;
    struct dd_params p;
    int32_t block[DD_BLOCK_SIZE];
    FILE *f = fopen("out/benchmark-voice.wav", "wb");
    uint32_t remaining = 48000;
    int trigger = 1;

    p.p[0] = 2000;
    p.p[1] = 28000;
    p.p[2] = 16000;
    p.p[3] = 20000;
    p.p[4] = 0;
    p.p[5] = 28000;
    p.p[6] = 16000;
    p.level = 28000;

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

    benchmark_voice_init(&voice);
    while (remaining) {
        uint32_t n = remaining > DD_BLOCK_SIZE ? DD_BLOCK_SIZE : remaining;
        uint32_t i;
        benchmark_voice_render(&voice, &p, trigger, block, n);
        trigger = 0;
        for (i = 0; i < n; ++i)
            u16le(f, (uint16_t)(block[i] >> 16));
        remaining -= n;
    }
    fclose(f);
    printf("ok: benchmark voice rendered to out/benchmark-voice.wav\n");
    return 0;
}
