/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dd_noise.h"
#include "dd_fixed.h"

static void test_determinism(void)
{
    dd_noise n1, n2;
    int32_t a[100], b[100];
    uint32_t i;

    dd_noise_init(&n1, 12345);
    dd_noise_init(&n2, 12345);
    for (i = 0; i < 100; ++i) {
        a[i] = dd_noise_q15(&n1);
        b[i] = dd_noise_q15(&n2);
    }
    assert(memcmp(a, b, sizeof(a)) == 0);
}

static void test_different_seeds(void)
{
    dd_noise n1, n2;
    int32_t a[10], b[10];
    uint32_t i;

    dd_noise_init(&n1, 1);
    dd_noise_init(&n2, 2);
    for (i = 0; i < 10; ++i) {
        a[i] = dd_noise_q15(&n1);
        b[i] = dd_noise_q15(&n2);
    }
    assert(memcmp(a, b, sizeof(a)) != 0);
}

static void test_q15_range(void)
{
    dd_noise n;
    dd_noise_init(&n, 42);
    for (uint32_t i = 0; i < 1000; ++i) {
        int32_t v = dd_noise_q15(&n);
        assert(v >= -32768 && v <= 32767);
    }
}

static void test_not_stuck(void)
{
    dd_noise n;
    dd_noise_init(&n, 7);
    int32_t first = dd_noise_q15(&n);
    uint32_t different = 0;
    for (uint32_t i = 0; i < 1000; ++i) {
        int32_t v = dd_noise_q15(&n);
        if (v != first)
            ++different;
    }
    assert(different > 0);
}

static void test_s16(void)
{
    dd_noise n;
    dd_noise_init(&n, 99);
    for (uint32_t i = 0; i < 100; ++i) {
        int16_t v = dd_noise_s16(&n);
        assert(v >= -32768 && v <= 32767);
    }
}

int main(void)
{
    test_determinism();
    test_different_seeds();
    test_q15_range();
    test_not_stuck();
    test_s16();
    printf("ok: noise\n");
    return 0;
}
