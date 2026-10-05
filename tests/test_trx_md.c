/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dd_trx_md.h"

enum { BLOCKS = 256, N = BLOCKS * DD_BLOCK_SIZE };

static dd_trx_params defaults(dd_trx_kind kind)
{
    dd_trx_params p = {
        { 8000, 24000, 16000, 12000, 12000, 10000, 8000, 10000 }, 28000
    };
    if (kind == DD_TRX_SD) {
        p.control[0] = 12000;
        p.control[4] = 18000;
        p.control[5] = 16000;
    }
    if (kind == DD_TRX_B2)
        p.control[3] = 4000;
    return p;
}

static uint32_t energy(const int32_t *samples, uint32_t count)
{
    uint32_t sum = 0, i;
    for (i = 0; i < count; ++i) {
        int32_t v = samples[i] >> 16;
        sum += (uint32_t)(v < 0 ? -v : v);
    }
    return sum;
}

static void render(dd_trx_voice *v, const dd_trx_params *p, int32_t *out)
{
    uint32_t i;
    for (i = 0; i < BLOCKS; ++i)
        dd_trx_render(v, p, i == 0, out + i * DD_BLOCK_SIZE, DD_BLOCK_SIZE);
}

static void test_kind(dd_trx_kind kind)
{
    static int32_t a[N], b[N], changed[N];
    dd_trx_voice va, vb, vc;
    dd_trx_params p = defaults(kind);
    uint32_t i;

    dd_trx_init(&va, kind);
    dd_trx_init(&vb, kind);
    render(&va, &p, a);
    render(&vb, &p, b);
    assert(memcmp(a, b, sizeof(a)) == 0);
    assert(energy(a, N / 2) > 1000);
    assert(energy(a, N / 4) > energy(a + 3 * N / 4, N / 4));
    for (i = 0; i < N; ++i) {
        assert(a[i] <= 0x7fff0000);
        assert(a[i] >= (int32_t)0x80010000);
    }
    for (i = 0; i < 8; ++i) {
        p = defaults(kind);
        p.control[i] = 0;
        dd_trx_init(&vc, kind);
        render(&vc, &p, changed);
        assert(memcmp(a, changed, sizeof(a)) != 0);
    }
    p = defaults(kind);
    p.level = 0;
    dd_trx_init(&vc, kind);
    render(&vc, &p, changed);
    assert(energy(changed, N) == 0);
}

static void test_eight_voices(void)
{
    dd_trx_voice voices[8];
    int32_t out[DD_BLOCK_SIZE];
    uint32_t i, block, sum = 0;
    for (i = 0; i < 8; ++i)
        dd_trx_init(&voices[i], (dd_trx_kind)(i % 3));
    for (block = 0; block < 64; ++block) {
        for (i = 0; i < 8; ++i) {
            dd_trx_params p = defaults((dd_trx_kind)(i % 3));
            dd_trx_render(&voices[i], &p, block == 0, out, DD_BLOCK_SIZE);
            sum += energy(out, DD_BLOCK_SIZE);
        }
    }
    assert(sum > 1000);
}

static void test_extremes(void)
{
    dd_trx_voice voice;
    dd_trx_params p;
    int32_t out[DD_BLOCK_SIZE];
    uint32_t kind, edge, i, block;
    for (kind = 0; kind < 3; ++kind) {
        for (edge = 0; edge < 2; ++edge) {
            for (i = 0; i < 8; ++i)
                p.control[i] = edge ? 32767 : 0;
            p.level = 32767;
            dd_trx_init(&voice, (dd_trx_kind)kind);
            for (block = 0; block < 128; ++block) {
                dd_trx_render(&voice, &p, block == 0, out, DD_BLOCK_SIZE);
                for (i = 0; i < DD_BLOCK_SIZE; ++i) {
                    assert(out[i] <= 0x7fff0000);
                    assert(out[i] >= (int32_t)0x80010000);
                }
            }
        }
    }
}

static void test_block_update_and_retrigger(void)
{
    dd_trx_voice voice;
    dd_trx_params p = defaults(DD_TRX_BD);
    int32_t out[DD_BLOCK_SIZE];
    uint32_t original_inc;

    dd_trx_init(&voice, DD_TRX_BD);
    dd_trx_render(&voice, &p, 1, out, DD_BLOCK_SIZE);
    original_inc = voice.body_inc;
    p.control[0] = 24000;
    dd_trx_render(&voice, &p, 0, out, DD_BLOCK_SIZE);
    assert(voice.body_inc != original_inc);
    assert(voice.amp.active);
    dd_trx_render(&voice, &p, 1, out, DD_BLOCK_SIZE);
    assert(voice.amp.active);
    assert(energy(out, DD_BLOCK_SIZE) > 0);
}

static void test_control_meanings(void)
{
    static int32_t short_hit[N], held_hit[N];
    dd_trx_voice a, b;
    dd_trx_params p = defaults(DD_TRX_B2);

    p.control[3] = 0;
    dd_trx_init(&a, DD_TRX_B2);
    render(&a, &p, short_hit);
    p.control[3] = 32767;
    dd_trx_init(&b, DD_TRX_B2);
    render(&b, &p, held_hit);
    assert(energy(held_hit + N / 2, N / 2) >
           energy(short_hit + N / 2, N / 2));

    p.control[6] = 0;
    dd_trx_init(&a, DD_TRX_B2);
    dd_trx_render(&a, &p, 1, short_hit, DD_BLOCK_SIZE);
    assert(a.bits == 12);
    p.control[6] = 32767;
    dd_trx_render(&a, &p, 0, short_hit, DD_BLOCK_SIZE);
    assert(a.bits == 2);

    p = defaults(DD_TRX_BD);
    p.control[3] = 0;
    dd_trx_init(&a, DD_TRX_BD);
    dd_trx_render(&a, &p, 1, short_hit, DD_BLOCK_SIZE);
    p.control[3] = 32767;
    dd_trx_init(&b, DD_TRX_BD);
    dd_trx_render(&b, &p, 1, held_hit, DD_BLOCK_SIZE);
    assert(b.ramp_coeff > a.ramp_coeff);

    p = defaults(DD_TRX_SD);
    p.control[6] = 0;
    dd_trx_init(&a, DD_TRX_SD);
    dd_trx_render(&a, &p, 1, short_hit, DD_BLOCK_SIZE);
    p.control[6] = 32767;
    dd_trx_init(&b, DD_TRX_SD);
    dd_trx_render(&b, &p, 1, held_hit, DD_BLOCK_SIZE);
    assert(b.second_inc > a.second_inc);
}

int main(void)
{
    test_kind(DD_TRX_BD);
    test_kind(DD_TRX_B2);
    test_kind(DD_TRX_SD);
    test_eight_voices();
    test_extremes();
    test_block_update_and_retrigger();
    test_control_meanings();
    printf("ok: TRX MD controls and voices\n");
    return 0;
}
