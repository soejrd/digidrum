/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "dd_fixed.h"

static void test_mul(void)
{
    assert(dd_mul_q15(16384, 32767) == 16383);
    assert(dd_mul_q15(0, 32767) == 0);
    assert(dd_mul_q15(-32767, 32767) == -32767);
    assert(dd_mul_q15(10000, 20000) == 6103);
}

static void test_abs(void)
{
    assert(dd_abs32(0) == 0);
    assert(dd_abs32(1) == 1);
    assert(dd_abs32(-1) == 1);
    assert(dd_abs32(32767) == 32767);
    assert(dd_abs32(-32767) == 32767);
}

static void test_clamp(void)
{
    assert(dd_clamp(50, 0, 100) == 50);
    assert(dd_clamp(-5, 0, 100) == 0);
    assert(dd_clamp(150, 0, 100) == 100);
}

static void test_clamp_q15(void)
{
    assert(dd_clamp_q15(0) == 0);
    assert(dd_clamp_q15(32767) == 32767);
    assert(dd_clamp_q15(40000) == 32767);
    assert(dd_clamp_q15(-32767) == -32767);
    assert(dd_clamp_q15(-40000) == -32767);
}

static void test_clamp_s16(void)
{
    assert(dd_clamp_s16(0) == 0);
    assert(dd_clamp_s16(32767) == 32767);
    assert(dd_clamp_s16(40000) == 32767);
    assert(dd_clamp_s16(-32768) == -32768);
    assert(dd_clamp_s16(-40000) == -32768);
}

static void test_sadd_ssub(void)
{
    assert(dd_sadd_q15(10000, 20000) == 30000);
    assert(dd_sadd_q15(30000, 10000) == 32767);
    assert(dd_sadd_q15(-30000, -10000) == -32767);
    assert(dd_ssub_q15(30000, 10000) == 20000);
    assert(dd_ssub_q15(30000, -10000) == 32767);
    assert(dd_ssub_q15(-30000, 10000) == -32767);
}

static void test_lerp(void)
{
    assert(dd_lerp_q15(0, 32767, 0) == 0);
    assert(dd_lerp_q15(0, 32767, 32767) == 32766);
    assert(dd_lerp_q15(0, 32767, 16384) == 16383);
    assert(dd_lerp_q15(-32767, 32767, 16384) == 0);
}

static void test_clip(void)
{
    assert(dd_soft_clip(0, 0) == 0);
    assert(dd_soft_clip(1000, 0) == 999);
    assert(dd_soft_clip(20000, 6000) > 10000);
    assert(dd_soft_clip(20000, 6000) <= 32767);
    assert(dd_soft_clip(-20000, 6000) >= -32767);
    assert(dd_soft_clip(-20000, 6000) < -10000);

    assert(dd_hard_clip(1000, 20000) == 1000);
    assert(dd_hard_clip(25000, 20000) == 20000);
    assert(dd_hard_clip(-25000, 20000) == -20000);
    assert(dd_hard_clip(-1000, 20000) == -1000);
}

static void test_quantizer(void)
{
    assert(dd_bit_quantize(1000, 16) == 1000);
    assert(dd_bit_quantize(1000, 8) == 768);
    assert(dd_bit_quantize(32767, 8) == 32512);
    assert(dd_bit_quantize(-32767, 8) == -32768);
    assert(dd_bit_quantize(32767, 4) == 28672);
    assert(dd_bit_quantize(32767, 1) == 0);
}

int main(void)
{
    test_mul();
    test_abs();
    test_clamp();
    test_clamp_q15();
    test_clamp_s16();
    test_sadd_ssub();
    test_lerp();
    test_clip();
    test_quantizer();
    printf("ok: fixed-point helpers\n");
    return 0;
}
