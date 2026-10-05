/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "dd_osc.h"
#include "dd_tables.h"
#include "dd_fixed.h"

static uint32_t phase_inc_for_freq(uint32_t freq_hz, uint32_t sample_rate)
{
    return (uint32_t)(((uint64_t)freq_hz << 32) / sample_rate);
}

static void test_sine_bounds(void)
{
    dd_osc osc;
    int32_t out;
    uint32_t i;
    dd_osc_init(&osc);
    uint32_t inc = phase_inc_for_freq(440, 48000);
    for (i = 0; i < 32; ++i) {
        out = dd_osc_sine(&osc, inc);
        assert(out >= -32767 && out <= 32767);
    }
}

static void test_sine_periodicity(void)
{
    dd_osc osc_a, osc_b;
    int32_t a[32], b[32];
    uint32_t i;
    dd_osc_init(&osc_a);
    dd_osc_init(&osc_b);
    uint32_t inc = phase_inc_for_freq(120, 48000);  // Changed from 110 to 120 for exact period
    uint32_t period_samples = 48000u / 120u;        // 400 samples exactly

    dd_osc_set_phase(&osc_a, 0);
    for (i = 0; i < period_samples; ++i)
        dd_osc_sine(&osc_a, inc);
    for (i = 0; i < 32; ++i)
        a[i] = dd_osc_sine(&osc_a, inc);

    dd_osc_set_phase(&osc_b, 0);
    for (i = 0; i < 32; ++i)
        b[i] = dd_osc_sine(&osc_b, inc);

    for (i = 0; i < 32; ++i)
        assert(a[i] == b[i]);
}

static void test_sine_interp_matches(void)
{
    dd_osc osc_a, osc_b;
    int32_t a, b;
    uint32_t i;
    dd_osc_init(&osc_a);
    dd_osc_init(&osc_b);
    uint32_t inc = phase_inc_for_freq(440, 48000);
    for (i = 0; i < 1000; ++i) {
        a = dd_osc_sine(&osc_a, inc);
        b = dd_osc_sine_interp(&osc_b, inc);
    }
    assert(dd_abs32(a - b) < 100);
}

static void test_square(void)
{
    dd_osc osc;
    uint32_t inc = phase_inc_for_freq(440, 48000);
    dd_osc_init(&osc);
    // Advance to get high output
    int32_t hi = dd_osc_square(&osc, inc, 0x80000000u);
    assert(hi == 32767);
    // Set phase to get low output on next call
    osc.phase = 0x80000000u - inc;
    int32_t lo = dd_osc_square(&osc, inc, 0x80000000u);
    assert(lo == -32767);

    dd_osc_init(&osc);
    uint32_t hi_count = 0;
    for (uint32_t i = 0; i < 1000; ++i) {
        if (dd_osc_square(&osc, inc, 0x80000000u) > 0)
            ++hi_count;
    }
    assert(hi_count > 0 && hi_count < 1000);
}

static void test_triangle_periodicity(void)
{
    dd_osc osc_a, osc_b;
    int32_t a[32], b[32];
    uint32_t i;
    dd_osc_init(&osc_a);
    dd_osc_init(&osc_b);
    uint32_t inc = phase_inc_for_freq(240, 48000);  // Changed from 220 to 240 for exact period
    uint32_t period = 48000u / 240u;                // 200 samples exactly

    dd_osc_set_phase(&osc_a, 0);
    for (i = 0; i < period; ++i)
        dd_osc_triangle(&osc_a, inc);
    for (i = 0; i < 32; ++i)
        a[i] = dd_osc_triangle(&osc_a, inc);

    dd_osc_set_phase(&osc_b, 0);
    for (i = 0; i < 32; ++i)
        b[i] = dd_osc_triangle(&osc_b, inc);

    for (i = 0; i < 32; ++i)
        assert(a[i] == b[i]);
}

static void test_saw_bounds(void)
{
    dd_osc osc;
    uint32_t inc = phase_inc_for_freq(440, 48000);
    dd_osc_init(&osc);
    for (uint32_t i = 0; i < 32; ++i) {
        int32_t s = dd_osc_saw(&osc, inc);
        assert(s >= -32767 && s <= 32767);
    }
}

int main(void)
{
    test_sine_bounds();
    test_sine_periodicity();
    test_sine_interp_matches();
    test_square();
    test_triangle_periodicity();
    test_saw_bounds();
    printf("ok: oscillators\n");
    return 0;
}
