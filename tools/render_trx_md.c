/* SPDX-License-Identifier: MIT */
#include <stdint.h>
#include <stdio.h>

#include "dd_trx_md.h"

static void u16le(FILE *f, uint16_t x)
{
    fputc(x & 0xff, f);
    fputc(x >> 8, f);
}

static void u32le(FILE *f, uint32_t x)
{
    u16le(f, (uint16_t)x);
    u16le(f, (uint16_t)(x >> 16));
}

static int render_one(const char *path, dd_trx_kind kind)
{
    dd_trx_params p = {
        { 8000, 24000, 16000, 12000, 12000, 10000, 8000, 10000 }, 28000
    };
    dd_trx_voice voice;
    int32_t block[DD_BLOCK_SIZE];
    FILE *f = fopen(path, "wb");
    uint32_t at, i;

    if (!f)
        return 1;
    if (kind == DD_TRX_SD) {
        p.control[0] = 12000;
        p.control[4] = 18000;
        p.control[5] = 16000;
    }
    if (kind == DD_TRX_B2)
        p.control[3] = 4000;
    fwrite("RIFF", 1, 4, f);
    u32le(f, 36u + 48000u * 2u);
    fwrite("WAVEfmt ", 1, 8, f);
    u32le(f, 16);
    u16le(f, 1);
    u16le(f, 1);
    u32le(f, 48000);
    u32le(f, 96000);
    u16le(f, 2);
    u16le(f, 16);
    fwrite("data", 1, 4, f);
    u32le(f, 48000u * 2u);

    dd_trx_init(&voice, kind);
    for (at = 0; at < 48000; at += DD_BLOCK_SIZE) {
        dd_trx_render(&voice, &p, at == 0, block, DD_BLOCK_SIZE);
        for (i = 0; i < DD_BLOCK_SIZE; ++i)
            u16le(f, (uint16_t)(block[i] >> 16));
    }
    if (fclose(f) != 0)
        return 1;
    printf("ok: %s\n", path);
    return 0;
}

int main(void)
{
    return render_one("out/trx-bd.wav", DD_TRX_BD) ||
           render_one("out/trx-b2.wav", DD_TRX_B2) ||
           render_one("out/trx-sd.wav", DD_TRX_SD);
}
