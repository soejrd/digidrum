/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "dd_tables.h"
#include "dd_fixed.h"

static void test_sine_table(void)
{
    assert(dd_sine_tab[0] == 0);
    assert(dd_sine_tab[DD_SINE_SIZE / 4] == 32767);
    assert(dd_sine_tab[DD_SINE_SIZE / 2] == 0);
    assert(dd_sine_tab[(DD_SINE_SIZE * 3) / 4] == -32767);
    assert(dd_sine_tab[DD_SINE_SIZE - 1] == -402);
    assert(dd_sine_tab[DD_SINE_SIZE - 1] == -dd_sine_tab[1]);
}

static void test_sine_monotonic(void)
{
    uint32_t i;
    // Check first quarter (0 to π/2) is monotonically increasing
    for (i = 1; i < DD_SINE_SIZE / 4; ++i) {
        assert(dd_sine_tab[i] > dd_sine_tab[i - 1]);
    }
    // Check second quarter (π/2 to π) is monotonically decreasing
    for (i = DD_SINE_SIZE / 4 + 1; i < DD_SINE_SIZE / 2; ++i) {
        assert(dd_sine_tab[i] < dd_sine_tab[i - 1]);
    }
}

static void test_decay_table(void)
{
    assert(dd_exp_decay_tab[0] < dd_exp_decay_tab[DD_EXP_SIZE - 1]);
    uint32_t i;
    for (i = 1; i < DD_EXP_SIZE; ++i) {
        assert(dd_exp_decay_tab[i] >= dd_exp_decay_tab[i - 1]);
    }
    assert(dd_exp_decay_tab[0] > 0);
    assert(dd_exp_decay_tab[DD_EXP_SIZE - 1] <= 32767);
}

static void test_exp_decay_to_coeff(void)
{
    uint16_t c0 = dd_exp_decay_to_coeff(0);
    uint16_t c64 = dd_exp_decay_to_coeff(64);
    uint16_t c127 = dd_exp_decay_to_coeff(127);
    assert(c0 > 0 && c0 < 32767);
    assert(c64 > c0);
    assert(c127 > c64);
    assert(c127 <= 32767);
}

static void test_decay_coeff_from_ms(void)
{
    int32_t fast = dd_decay_coeff_from_ms(1);
    int32_t slow = dd_decay_coeff_from_ms(1000);
    int32_t mid = dd_decay_coeff_from_ms(50);
    assert(fast > 0);
    assert(slow > fast);
    assert(mid > fast && mid < slow);
    assert(dd_decay_coeff_from_ms(0) == 0);
}

int main(void)
{
    test_sine_table();
    test_sine_monotonic();
    test_decay_table();
    test_exp_decay_to_coeff();
    test_decay_coeff_from_ms();
    printf("ok: tables\n");
    return 0;
}
