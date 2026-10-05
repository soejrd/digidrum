/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../percussion.h"

static struct dp_params defaults(void)
{
    struct dp_params p;
    p.frequency = 214;
    p.tone = 18000;
    p.decay = 22000;
    p.attack_fm = 26000;
    p.self_fm = 9000;
    p.drive = 6000;
    p.accent = 26000;
    p.level = 28000;
    return p;
}

static uint64_t energy(const int32_t *x, uint32_t n)
{
    uint64_t sum = 0;
    uint32_t i;
    for (i = 0; i < n; ++i) {
        int32_t s = x[i] >> 16;
        sum += (uint32_t)(s < 0 ? -s : s);
    }
    return sum;
}

static void render(struct dp_voice *v, const struct dp_params *p,
                   int32_t *out, uint32_t n)
{
    uint32_t at = 0;
    while (at < n) {
        uint32_t block = n - at > DP_BLOCK_SIZE ? DP_BLOCK_SIZE : n - at;
        dp_voice_render(v, p, at == 0, out + at, block);
        at += block;
    }
}

int main(void)
{
    enum { N = 48000 };
    static int32_t a[N];
    static int32_t b[N];
    struct dp_params p = defaults();
    struct dp_voice va;
    struct dp_voice vb;
    uint64_t early;
    uint64_t late;
    uint32_t pitch;
    uint32_t control;
    struct dp_voice eight[8];
    struct dp_params eight_params[8];
    int32_t eight_out[8][DP_BLOCK_SIZE];
    uint32_t track;
    uint32_t block;
    uint64_t eight_energy[8] = {0};

    dp_voice_init(&va);
    render(&va, &p, a, N);
    early = energy(a, 12000);
    late = energy(a + 36000, 12000);
    assert(early > 100000);
    assert(late < early);

    dp_voice_init(&vb);
    render(&vb, &p, b, N);
    assert(memcmp(a, b, sizeof a) == 0);

    p.attack_fm = 0;
    dp_voice_init(&vb);
    render(&vb, &p, b, N);
    assert(memcmp(a, b, 4096 * sizeof *a) != 0);

    memset(b, 0x55, sizeof b);
    dp_voice_init(&vb);
    dp_voice_render(&vb, &p, 0, b, DP_BLOCK_SIZE);
    assert(energy(b, DP_BLOCK_SIZE) == 0);

    /* Sweep the exposed ranges at deliberately unfriendly corners. Besides
     * catching instability, this exercises every multiply at its maximum. */
    for (pitch = 64; pitch <= 3000; pitch += 97) {
        for (control = 0; control <= 32767; control += 4095) {
            uint32_t i;
            p.frequency = (uint16_t)pitch;
            p.tone = (uint16_t)control;
            p.decay = (uint16_t)(32767 - control);
            p.attack_fm = (uint16_t)control;
            p.self_fm = (uint16_t)(32767 - control);
            p.drive = (uint16_t)control;
            p.accent = 32767;
            p.level = 32767;
            dp_voice_init(&vb);
            dp_voice_render(&vb, &p, 1, b, DP_BLOCK_SIZE);
            for (i = 0; i < DP_BLOCK_SIZE; ++i)
                assert(b[i] <= 0x7fff0000 && b[i] >= (int32_t)0x80010000);
        }
    }

    /* Eight tracks may strike in the same render block. Each voice must
     * produce bounded independent output across overlapping tails. This is
     * a host DSP check, not a Digitakt CPU benchmark or sequencer test. */
    for (track = 0; track < 8; ++track) {
        eight_params[track] = defaults();
        eight_params[track].frequency = (uint16_t)(110 + track * 30);
        dp_voice_init(&eight[track]);
    }
    for (block = 0; block < 64; ++block) {
        for (track = 0; track < 8; ++track) {
            uint32_t i;
            dp_voice_render(&eight[track], &eight_params[track],
                            block == 0, eight_out[track], DP_BLOCK_SIZE);
            eight_energy[track] += energy(eight_out[track], DP_BLOCK_SIZE);
            for (i = 0; i < DP_BLOCK_SIZE; ++i)
                assert(eight_out[track][i] <= 0x7fff0000 &&
                       eight_out[track][i] >= (int32_t)0x80010000);
        }
    }
    for (track = 0; track < 8; ++track)
        assert(eight_energy[track] > 1000);
    assert(memcmp(eight_out[0], eight_out[7], sizeof eight_out[0]) != 0);

    printf("ok: early=%llu late=%llu\n",
           (unsigned long long)early,
           (unsigned long long)late);
    return 0;
}
