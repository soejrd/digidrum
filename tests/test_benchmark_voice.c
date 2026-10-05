/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "benchmark_voice.h"
#include "dd_fixed.h"
#include "dd_machine.h"
#include "dd_tables.h"

static uint32_t energy(const int32_t *x, uint32_t n)
{
    uint32_t sum = 0;
    for (uint32_t i = 0; i < n; ++i) {
        int32_t s = x[i] >> 16;
        sum += (uint32_t)(s < 0 ? -s : s);
    }
    return sum;
}

static struct dd_params defaults(void)
{
    struct dd_params p;
    p.p[0] = 2000;
    p.p[1] = 28000;
    p.p[2] = 16000;
    p.p[3] = 20000;
    p.p[4] = 0;
    p.p[5] = 28000;
    p.p[6] = 16000;
    p.level = 28000;
    return p;
}

int main(void)
{
    enum { N = 48000 };
    static int32_t a[N];
    static int32_t b[N];
    struct benchmark_voice va, vb;
    struct dd_params p = defaults();

    benchmark_voice_init(&va);
    benchmark_voice_init(&vb);

    for (uint32_t at = 0; at < N; at += DD_BLOCK_SIZE) {
        uint32_t n = (N - at > DD_BLOCK_SIZE) ? DD_BLOCK_SIZE : (N - at);
        benchmark_voice_render(&va, &p, at == 0, a + at, n);
        benchmark_voice_render(&vb, &p, at == 0, b + at, n);
    }

    assert(energy(a, 48000) > 1000);
    assert(memcmp(a, b, sizeof(a)) == 0);

    for (uint32_t i = 0; i < N; ++i) {
        assert(a[i] <= 0x7fff0000);
        assert(a[i] >= (int32_t)0x80010000);
    }

    uint32_t early = energy(a, 12000);
    uint32_t late = energy(a + 36000, 12000);
    assert(early > late);

    benchmark_voice_init(&va);
    benchmark_voice_render(&va, &p, 1, a, DD_BLOCK_SIZE);
    assert(va.params.valid);
    assert(va.body_inc_base == 4000000u + (uint32_t)p.p[DD_PITCH] * 8000u);
    assert(va.amp_env.coeff == dd_exp_decay_to_coeff(p.p[DD_DECAY] >> 8));

    p.p[DD_PITCH] += 100;
    p.p[DD_DECAY] = 1000;
    p.level = 16000;
    benchmark_voice_render(&va, &p, 0, a, DD_BLOCK_SIZE);
    assert(va.body_inc_base == 4000000u + (uint32_t)p.p[DD_PITCH] * 8000u);
    assert(va.amp_env.coeff == dd_exp_decay_to_coeff(p.p[DD_DECAY] >> 8));
    assert(va.level == 16000);
    assert(va.amp_env.active);

    printf("ok: benchmark voice deterministic, bounded, decaying (early=%u late=%u)\n",
           early, late);
    return 0;
}
