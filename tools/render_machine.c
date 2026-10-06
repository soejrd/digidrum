/* SPDX-License-Identifier: MIT */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dd_trx_md.h"

static void le16(FILE *f, uint16_t v) { fputc(v & 255, f); fputc(v >> 8, f); }
static void le32(FILE *f, uint32_t v) { le16(f, (uint16_t)v); le16(f, (uint16_t)(v >> 16)); }

static int number(const char *s, uint32_t max, uint32_t *out)
{
    char *end;
    unsigned long v = strtoul(s, &end, 10);
    if (!*s || *end || v > max) return 0;
    *out = (uint32_t)v;
    return 1;
}

static int params(const char *s, dd_trx_params *p)
{
    uint32_t i, v;
    for (i = 0; i < 8; ++i) {
        char *end;
        unsigned long n = strtoul(s, &end, 10);
        if (end == s || n > 127 || (i < 7 ? *end != ',' : *end != 0)) return 0;
        v = (uint32_t)n;
        p->control[i] = (uint16_t)((v * 32767u + 63u) / 127u);
        s = end + (i < 7);
    }
    p->level = 32767;
    return 1;
}

int main(int argc, char **argv)
{
    dd_trx_voice voice;
    dd_trx_params p = {{0}, 32767};
    dd_trx_kind kind = DD_TRX_B2;
    const char *output = NULL;
    uint32_t frames = 96000, trigger = 12000, i, n;
    int have_params = 0;
    int32_t block[DD_BLOCK_SIZE];
    FILE *f;
    for (i = 1; i < (uint32_t)argc; i += 2) {
        if (i + 1 >= (uint32_t)argc) goto usage;
        if (!strcmp(argv[i], "--machine")) {
            if (!strcmp(argv[i+1], "trx_b2")) kind = DD_TRX_B2;
            else if (!strcmp(argv[i+1], "trx_bd")) kind = DD_TRX_BD;
            else if (!strcmp(argv[i+1], "trx_sd")) kind = DD_TRX_SD;
            else goto usage;
        } else if (!strcmp(argv[i], "--params")) {
            if (!params(argv[i+1], &p)) goto usage;
            have_params = 1;
        } else if (!strcmp(argv[i], "--frames")) {
            if (!number(argv[i+1], 10000000u, &frames) || !frames) goto usage;
        } else if (!strcmp(argv[i], "--trigger-frame")) {
            if (!number(argv[i+1], 10000000u, &trigger)) goto usage;
        } else if (!strcmp(argv[i], "--output")) output = argv[i+1];
        else goto usage;
    }
    if (!output || !have_params || trigger >= frames || frames > (UINT32_MAX - 36u) / 2u) goto usage;
    f = fopen(output, "wb");
    if (!f) { perror(output); return 1; }
    fwrite("RIFF", 1, 4, f); le32(f, 36u + frames * 2u);
    fwrite("WAVEfmt ", 1, 8, f); le32(f, 16); le16(f, 1); le16(f, 1);
    le32(f, 48000); le32(f, 96000); le16(f, 2); le16(f, 16);
    fwrite("data", 1, 4, f); le32(f, frames * 2u);
    dd_trx_init(&voice, kind);
    for (i = 0; i < frames; i += n) {
        n = frames - i;
        if (n > DD_BLOCK_SIZE) n = DD_BLOCK_SIZE;
        if (i < trigger && i + n > trigger) n = trigger - i;
        dd_trx_render(&voice, &p, i == trigger, block, n);
        for (uint32_t j = 0; j < n; ++j) le16(f, (uint16_t)(block[j] >> 16));
    }
    if (fclose(f)) { perror(output); return 1; }
    return 0;
usage:
    fprintf(stderr, "usage: render_machine --machine trx_b2|trx_bd|trx_sd --params v0,...,v7 --frames N --trigger-frame N --output file.wav\n");
    return 2;
}
