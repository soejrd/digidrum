/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dd_rate.h"
#include "dd_fixed.h"

static void test_zoh(void)
{
    dd_zoh z;
    dd_zoh_init(&z, 1000);
    int32_t a = dd_zoh_step(&z, 2000);
    int32_t b = dd_zoh_step(&z, 3000);
    assert(a == 1000);
    assert(b == 2000);
    int32_t c = dd_zoh_step(&z, 4000);
    assert(c == 3000);
}

static void test_interp_zero_rate(void)
{
    dd_interp i;
    dd_interp_init(&i, 0, 0);
    int32_t a = dd_interp_step(&i, 0);
    int32_t b = dd_interp_step(&i, 32767);
    assert(a == 0);
    assert(b == 0);
}

static void test_interp_half_rate(void)
{
    dd_interp i;
    dd_interp_init(&i, 0, 16384);
    int32_t a = dd_interp_step(&i, 32767);
    assert(a == 16383);
    int32_t b = dd_interp_step(&i, 32767);
    assert(b == 32767);
}

static void test_downsampler_even_rate(void)
{
    dd_downsampler d;
    dd_downsampler_init(&d);
    for (uint32_t i = 0; i < 10; ++i)
        assert(dd_downsampler_should_process(&d) == (i % 2 == 1));
    assert(d.tick == 0);
}

static void test_downsampler_quarter_rate(void)
{
    dd_downsampler d;
    dd_downsampler_init_rate(&d, 4);
    for (uint32_t i = 0; i < 12; ++i)
        assert(dd_downsampler_should_process(&d) == (i % 4 == 3));
    assert(d.tick == 0);
}

static void test_zoh_holds_value(void)
{
    dd_zoh z;
    dd_zoh_init(&z, 5000);
    for (uint32_t i = 0; i < 10; ++i) {
        int32_t v = dd_zoh_step(&z, 9000);
        assert(v == (i == 0 ? 5000 : 9000));
    }
}

int main(void)
{
    test_zoh();
    test_interp_zero_rate();
    test_interp_half_rate();
    test_downsampler_even_rate();
    test_downsampler_quarter_rate();
    test_zoh_holds_value();
    printf("ok: rate / interpolation\n");
    return 0;
}
