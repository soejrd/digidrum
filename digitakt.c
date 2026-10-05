/* SPDX-License-Identifier: GPL-2.0-or-later */
/* Digitakt Mk1 OS 1.53 adapter for the portable percussion kernel. */
#include "percussion.h"
#include "include/dd_trx_md.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef int16_t s16;
typedef int32_t s32;
typedef uint32_t u32;

#define DP_MACHINE 8
#define TRX_BD_MACHINE 9
#define TRX_B2_MACHINE 10
#define TRX_SD_MACHINE 11
#define TRACKS 8
#define TBUF(t) ((s32 *)(uintptr_t)(0x80001a18u + 128u * (u32)(t)))
#define MACH(t) (*(volatile const u8 *)(uintptr_t)(0x800018bcu + (u32)(t)))
#define VP(t, o) (*(volatile const s16 *)(uintptr_t)(0x80002794u + 106u * (u32)(t) + (u32)(o)))
#define NOTE(t) (*(volatile const s32 *)(uintptr_t)(0x80001f28u + 4u * (u32)(t)))
#define VEL(t) (*(volatile const s16 *)(uintptr_t)(0x80001f18u + 2u * (u32)(t)))
#define TRIG_BITS (*(volatile const u32 *)(uintptr_t)0x80001228u)
#define PITCH_TAB ((const u32 *)(uintptr_t)0x4019b1c0u)

#define P_TUNE 0
#define P_PLAY 2
#define P_BR 4
#define P_SAMP 6
#define P_SLICE 8
#define P_LEN 10
#define P_GRID 12
#define P_LEV 14

static struct dp_voice dp_voices[TRACKS];
static dd_trx_voice trx_voices[TRACKS];
static u8 last_machine[TRACKS];

/* The SRC view treats slot D as the sample parameter. For PULSE BD, pass
 * only that slot through the stock generic setter at 0x40030a28. */
typedef s32 (*dp_pageset_t)(void *view, s32 param, s32 delta, s32 flag,
                            u8 *changed);
typedef s32 (*dp_trackof_t)(void *track_ref);
typedef s32 (*dp_machineof_t)(void *view);
typedef void *(*dp_paramof_t)(void *view, s32 param, s32 group);
typedef s32 (*dp_paramset_t)(void *object, s32 param, s32 delta, s32 track,
                              s32 flag, u8 *changed, s32 notify, s32 redraw);

extern u32 dp_page_m;

void dp_pageset(void *view, s32 param, s32 delta, s32 flag, u8 *changed)
{
    dp_pageset_t stock = (dp_pageset_t)(uintptr_t)0x400309b0u;
    s32 machine = ((dp_machineof_t)(uintptr_t)0x4002b5d4u)(view);
    if (param == 10 && machine >= DP_MACHINE && machine <= TRX_SD_MACHINE) {
        void **view_vt = *(void ***)view;
        void *track_ref = *(void **)((u8 *)view + 116);
        s32 track = ((dp_trackof_t)(uintptr_t)0x4001d24eu)(track_ref);
        void *object = ((dp_paramof_t)view_vt[41])(view, param, -1);
        void **object_vt = *(void ***)object;
        ((dp_paramset_t)object_vt[11])(
            object, param, delta, track, flag, changed, 1, 1);
        return;
    }
    stock(view, param, delta, flag, changed);
}

/* SRC-page state shared with glue.s.  The firmware asks for a page layout
 * immediately before drawing or editing it, so dp_page_m identifies whether
 * the otherwise-global parameter UI hooks belong to PULSE BD. */
u32 dp_lay[11];
u32 dp_lay_ok;
u32 dp_page_m;
char dp_txt[12];

char *dp_fmt_character(char *out, s32 value)
{
    static const char names[4][6] = { "CLEAN", "SWEEP", "PUNCH", "BOTH" };
    u32 index = (u32)value >> 8;
    u32 i = 0;
    if (index > 3u)
        index = 3u;
    while (names[index][i]) {
        out[i] = names[index][i];
        ++i;
    }
    out[i] = 0;
    return out;
}

static u32 dp_pitch_ratio(s32 track)
{
    s32 pitch = ((s32)VP(track, P_TUNE) - 0x4000) * 256
        + NOTE(track) + (3 << 16);
    if (pitch < 0)
        pitch = 0;
    else if (pitch > (87 << 16))
        pitch = 87 << 16;
    return PITCH_TAB[(u32)pitch / 384u];
}

static u32 dp_q15_from_u7(u32 value)
{
    if (value > 127u)
        value = 127u;
    return (value * 32767u + 63u) / 127u;
}

static u32 dp_param_u8(s32 track, s32 offset)
{
    return ((u32)(u16)VP(track, offset) >> 8) & 0xffu;
}

static void dp_read_params(s32 track, struct dp_params *p)
{
    u32 ratio = dp_pitch_ratio(track);
    u32 character = dp_param_u8(track, P_PLAY) & 3u;
    u32 drive = dp_param_u8(track, P_GRID);
    u32 velocity = ((u32)(u16)VEL(track) >> 8) & 0xffu;

    /* 214 is 2*sin(pi*50 Hz/48000) in Q15. PITCH_TAB is Q29. */
    p->frequency = (u16)((((ratio >> 12) * 214u) >> 17) & 0xffffu);
    if (p->frequency < 32u)
        p->frequency = 32u;
    else if (p->frequency > 3000u)
        p->frequency = 3000u;
    p->tone = (u16)dp_q15_from_u7(dp_param_u8(track, P_BR));
    p->decay = (u16)dp_q15_from_u7(dp_param_u8(track, P_LEN));
    p->attack_fm = character & 1u
        ? (u16)dp_q15_from_u7(dp_param_u8(track, P_SLICE)) : 0;
    p->self_fm = character & 2u
        ? (u16)dp_q15_from_u7(dp_param_u8(track, P_SAMP)) : 0;
    p->drive = (u16)dp_q15_from_u7(drive);
    p->accent = (u16)dp_q15_from_u7(velocity);
    p->level = (u16)dp_q15_from_u7(dp_param_u8(track, P_LEV));
}

static void dp_read_trx_params(s32 track, dd_trx_params *p)
{
    u32 i;
    for (i = 0; i < 8u; ++i)
        p->control[i] = (u16)dp_q15_from_u7(dp_param_u8(track, (s32)(i * 2u)));
    /* SRC H is the eighth synth control. The stock track AMP path follows
     * our injection and supplies the user-facing track level. */
    p->level = 32767;
}

void dp_inject(void)
{
    u32 triggers = TRIG_BITS;
    s32 track;
    for (track = 0; track < TRACKS; ++track) {
        struct dp_params params;
        dd_trx_params trx_params;
        s32 *output;
        u8 machine = MACH(track);

        if (machine != last_machine[track]) {
            if (machine == DP_MACHINE)
                dp_voice_init(&dp_voices[track]);
            else if (machine >= TRX_BD_MACHINE && machine <= TRX_SD_MACHINE)
                dd_trx_init(&trx_voices[track], (dd_trx_kind)(machine - TRX_BD_MACHINE));
            last_machine[track] = machine;
        }
        if (machine < DP_MACHINE || machine > TRX_SD_MACHINE) {
            continue;
        }
        output = TBUF(track);
        if (machine == DP_MACHINE) {
            dp_read_params(track, &params);
            dp_voice_render(&dp_voices[track], &params,
                            (triggers & (1u << track)) != 0,
                            output, DP_BLOCK_SIZE);
        } else {
            dp_read_trx_params(track, &trx_params);
            dd_trx_render(&trx_voices[track], &trx_params,
                          (triggers & (1u << track)) != 0,
                          output, DP_BLOCK_SIZE);
        }
    }
}
