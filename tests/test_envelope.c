/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dd_envelope.h"
#include "dd_fixed.h"

static void test_decay(void)
{
    dd_decay_env e;
    dd_decay_env_init(&e);
    assert(e.value == 0);
    assert(e.active == 0);

    dd_decay_env_set_coeff(&e, 32700);
    dd_decay_env_trigger(&e);
    assert(e.value == 32767);
    assert(e.active == 1);

    int32_t prev = e.value;
    e.value = dd_decay_env_step(&e);
    assert(e.value < prev);
    assert(e.value > 0);

    uint32_t i;
    prev = e.value;
    for (i = 0; i < 5000; ++i) {
        int32_t v = dd_decay_env_step(&e);
        assert(v <= prev);
        prev = v;
    }
    assert(e.active == 0);
    assert(e.value == 0);
}

static void test_decay_determinism(void)
{
    dd_decay_env e1, e2;
    int32_t buf1[32], buf2[32];
    uint32_t i;

    dd_decay_env_init(&e1);
    dd_decay_env_init(&e2);
    dd_decay_env_set_coeff(&e1, 32500);
    dd_decay_env_set_coeff(&e2, 32500);
    dd_decay_env_trigger(&e1);
    dd_decay_env_trigger(&e2);

    for (i = 0; i < 32; ++i) {
        buf1[i] = dd_decay_env_step(&e1);
        buf2[i] = dd_decay_env_step(&e2);
    }
    assert(memcmp(buf1, buf2, sizeof(buf1)) == 0);
}

static void test_ahd(void)
{
    dd_ahd_env e;
    dd_ahd_env_init(&e);
    assert(e.active == 0);

    dd_ahd_env_trigger(&e, 32767, 32000, 32700, 10);
    assert(e.stage == DD_AHD_ATTACK);
    assert(e.active == 1);

    uint32_t attack_steps = 0;
    int32_t peak = 0;
    while (e.active && attack_steps < 100) {
        int32_t v = dd_ahd_env_step(&e);
        if (v > peak) peak = v;
        if (e.stage == DD_AHD_ATTACK)
            ++attack_steps;
        if (e.stage == DD_AHD_HOLD)
            break;
    }
    assert(peak == 32767);

    assert(e.stage == DD_AHD_HOLD);
    assert(e.hold_count == 10);

    for (uint32_t i = 0; i < 15; ++i) {
        dd_ahd_env_step(&e);
    }
    assert(e.stage == DD_AHD_DECAY);

    uint32_t decay_steps = 0;
    int32_t prev = e.value;
    while (e.active && decay_steps < 5000) {
        int32_t v = dd_ahd_env_step(&e);
        assert(v <= prev + 1);
        prev = v;
        ++decay_steps;
    }
    assert(e.active == 0);
}

static void test_pitch_sweep(void)
{
    dd_pitch_sweep_env e;
    dd_pitch_sweep_init(&e);
    assert(e.active == 0);

    dd_pitch_sweep_trigger(&e, 1000, 8000, 32600);
    assert(e.active == 1);

    int32_t first = dd_pitch_sweep_step(&e);
    assert(first > 1000);
    assert(first < 8000);

    int32_t prev = first;
    uint32_t i;
    for (i = 0; i < 500; ++i) {
        int32_t v = dd_pitch_sweep_step(&e);
        assert(v >= 1000 && v <= 8000);
        assert(v <= prev);
        prev = v;
    }
    assert(e.active == 1);
    for (i = 0; e.active && i < 3000; ++i) {
        int32_t v = dd_pitch_sweep_step(&e);
        assert(v >= 1000 && v <= prev);
        prev = v;
    }
    assert(e.active == 0);
    assert(dd_pitch_sweep_step(&e) == 1000);

    dd_pitch_sweep_trigger(&e, 8000, 1000, 32600);
    prev = dd_pitch_sweep_step(&e);
    assert(prev > 1000 && prev < 8000);
    for (i = 0; e.active && i < 3000; ++i) {
        int32_t v = dd_pitch_sweep_step(&e);
        assert(v >= prev && v <= 8000);
        prev = v;
    }
    assert(e.active == 0);
    assert(dd_pitch_sweep_step(&e) == 8000);

    dd_pitch_sweep_trigger(&e, 1000, 8000, 0);
    assert(dd_pitch_sweep_step(&e) == 1000);
    assert(e.active == 0);
    dd_pitch_sweep_trigger(&e, 1000, 1000, 32600);
    assert(e.active == 0);
}

int main(void)
{
    test_decay();
    test_decay_determinism();
    test_ahd();
    test_pitch_sweep();
    printf("ok: envelopes\n");
    return 0;
}
