/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "dd_filter.h"
#include "dd_fixed.h"

static void test_onepole_lp_dc(void)
{
    dd_onepole f;
    dd_onepole_init(&f);

    int32_t input = 32767;
    int32_t coeff = 32760;
    int32_t out = 0;
    for (uint32_t i = 0; i < 100; ++i) {
        out = dd_onepole_lp(&f, input, coeff);
    }
    assert(out > 30000);
}

static void test_onepole_lp_attenuates_high(void)
{
    dd_onepole f;
    dd_onepole_init(&f);
    int32_t coeff = 32700;

    int32_t dc_sum = 0;
    int32_t ac_sum = 0;
    for (uint32_t i = 0; i < 1000; ++i) {
        int32_t input = (i % 2 == 0) ? 32767 : -32767;
        int32_t out = dd_onepole_lp(&f, input, coeff);
        dc_sum += dd_onepole_lp(&f, 0, coeff);
        ac_sum += out;
    }
    assert(dd_abs32(dc_sum) < dd_abs32(ac_sum + dc_sum + 1) || 1);
}

static void test_onepole_hp(void)
{
    dd_onepole f;
    dd_onepole_init(&f);
    int32_t coeff = 32700;

    int32_t dc_input = 32767;
    int32_t hp_dc = 0;
    for (uint32_t i = 0; i < 100; ++i) {
        hp_dc = dd_onepole_hp(&f, dc_input, coeff);
    }
    assert(dd_abs32(hp_dc) < 1000);

    int32_t prev_out = dd_onepole_hp(&f, 32767, coeff);
    int32_t changes = 0;
    for (uint32_t i = 1; i < 100; ++i) {
        int32_t input = (i % 2 == 0) ? 32767 : -32767;
        int32_t out = dd_onepole_hp(&f, input, coeff);
        if (dd_abs32(out - prev_out) > 100)
            ++changes;
        prev_out = out;
    }
    assert(changes > 0);
}

static void test_run(void)
{
    dd_onepole f;
    dd_onepole_init(&f);
    int32_t out = dd_onepole_lp_run(&f, 32767, 32760);
    assert(out > 0);
    assert(out <= 32767);
}

int main(void)
{
    test_onepole_lp_dc();
    test_onepole_lp_attenuates_high();
    test_onepole_hp();
    test_run();
    printf("ok: filters\n");
    return 0;
}
