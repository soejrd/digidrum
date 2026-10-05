/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dd_resonator.h"
#include "dd_fixed.h"

static void test_bounded_output(void)
{
    dd_resonator r;
    dd_resonator_init(&r);
    dd_resonator_set(&r, 214, 16384);

    int32_t out;
    for (uint32_t i = 0; i < 32; ++i) {
        out = dd_resonator_lp(&r, 32767);
        assert(out >= -65535 && out <= 65535);
    }
}

static void test_impulse_response_deterministic(void)
{
    dd_resonator r1, r2;
    int32_t out1[64], out2[64];

    dd_resonator_init(&r1);
    dd_resonator_init(&r2);
    dd_resonator_set(&r1, 214, 16384);
    dd_resonator_set(&r2, 214, 16384);

    out1[0] = dd_resonator_lp(&r1, 32767);
    out2[0] = dd_resonator_lp(&r2, 32767);
    for (uint32_t i = 1; i < 64; ++i) {
        out1[i] = dd_resonator_lp(&r1, 0);
        out2[i] = dd_resonator_lp(&r2, 0);
    }
    assert(memcmp(out1, out2, sizeof(out1)) == 0);
}

static void test_resonance_decay(void)
{
    dd_resonator r;
    dd_resonator_init(&r);
    dd_resonator_set(&r, 214, 16384);

    dd_resonator_lp(&r, 32767);
    int32_t initial = dd_resonator_lp(&r, 0);

    for (uint32_t i = 0; i < 500; ++i)
        dd_resonator_lp(&r, 0);
    int32_t final = dd_resonator_lp(&r, 0);

    assert(dd_abs32(final) < dd_abs32(initial) || dd_abs32(final) < 100);
}

static void test_bp_and_hp(void)
{
    dd_resonator r;
    dd_resonator_init(&r);
    dd_resonator_set(&r, 214, 16384);

    int32_t low = dd_resonator_lp(&r, 32767);
    assert(dd_abs32(low) <= 65535);
    dd_resonator_init(&r);
    dd_resonator_set(&r, 214, 16384);
    int32_t band = dd_resonator_bp(&r, 32767);
    assert(dd_abs32(band) <= 65535);
    dd_resonator_init(&r);
    dd_resonator_set(&r, 214, 16384);
    int32_t high = dd_resonator_hp(&r, 32767);
    assert(dd_abs32(high) <= 65535);
}

int main(void)
{
    test_bounded_output();
    test_impulse_response_deterministic();
    test_resonance_decay();
    test_bp_and_hp();
    printf("ok: resonator\n");
    return 0;
}
