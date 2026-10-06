| SPDX-License-Identifier: GPL-2.0-or-later
| PULSE BD and TRX machine pages, Digitakt Mk1 OS 1.53 render hook.

        .section .run, "ax"

| Immediately after stock playback has filled every track buffer, before the
| overdrive stage. Preserve every register, call the portable engine adapter,
| then perform the displaced instruction.
        .globl  dp_inject_s
dp_inject_s:
        lea     -60(%sp), %sp
        movem.l %d0-%d7/%a0-%a6, (%sp)
        jsr     dp_inject
        movem.l (%sp), %d0-%d7/%a0-%a6
        lea     60(%sp), %sp
        lea     0x4199e444, %a4
        rts

| Core 2.1 descriptors use SLICE's eight stored SRC slots. Each independent
| render ID leaves an empty stock playback window for the injected voice.
        .equ    BMP_VT, 0x401b73b4
        .balign 4
        .globl  dp_machine
dp_machine:
        .long   8, dp_name, dp_short, dp_icon_bmp, 3, 8
        .globl  dp_machine_bd, dp_machine_b2, dp_machine_sd
        .globl  dp_machine_ch, dp_machine_oh, dp_machine_cy
        .globl  dp_machine_rs, dp_machine_cb, dp_machine_cl
dp_machine_bd:
        .long   9, dp_name_bd, dp_short_bd, dp_icon_bmp, 3, 9
dp_machine_b2:
        .long   10, dp_name_b2, dp_short_b2, dp_icon_bmp, 3, 10
dp_machine_sd:
        .long   11, dp_name_sd, dp_short_sd, dp_icon_bmp, 3, 11
dp_machine_ch:
        .long   12, dp_name_ch, dp_short_ch, dp_icon_bmp, 3, 12
dp_machine_oh:
        .long   13, dp_name_oh, dp_short_oh, dp_icon_bmp, 3, 13
dp_machine_cy:
        .long   14, dp_name_cy, dp_short_cy, dp_icon_bmp, 3, 14
dp_machine_rs:
        .long   15, dp_name_rs, dp_short_rs, dp_icon_bmp, 3, 15
dp_machine_cb:
        .long   16, dp_name_cb, dp_short_cb, dp_icon_bmp, 3, 16
dp_machine_cl:
        .long   17, dp_name_cl, dp_short_cl, dp_icon_bmp, 3, 17

dp_name:
        .asciz  "PULSE BD"
dp_short:
        .asciz  "PBD"
dp_name_bd:   .asciz "TRX-BD"
dp_short_bd:  .asciz "TBD"
dp_name_b2:   .asciz "TRX-B2"
dp_short_b2:  .asciz "TB2"
dp_name_sd:   .asciz "TRX-SD"
dp_short_sd:  .asciz "TSD"
dp_name_ch:   .asciz "TRX-CH"
dp_short_ch:  .asciz "TCH"
dp_name_oh:   .asciz "TRX-OH"
dp_short_oh:  .asciz "TOH"
dp_name_cy:   .asciz "TRX-CY"
dp_short_cy:  .asciz "TCY"
dp_name_rs:   .asciz "TRX-RS"
dp_short_rs:  .asciz "TRS"
dp_name_cb:   .asciz "TRX-CB"
dp_short_cb:  .asciz "TCB"
dp_name_cl:   .asciz "TRX-CL"
dp_short_cl:  .asciz "TCL"

| 11 x 7 stylised drum/body icon.
        .balign 4
dp_icon_bmp:
        .long   BMP_VT, 11, 7, 1, dp_icon_px, dp_icon_mask, 0
dp_icon_px:
        .long   0x38000000, 0x44000000, 0x82000000, 0x92000000
        .long   0xaa000000, 0x92000000, 0xaa000000, 0x92000000
        .long   0x82000000, 0x44000000, 0x38000000
dp_icon_mask:
        .long   0xfe000000, 0xfe000000, 0xfe000000, 0xfe000000
        .long   0xfe000000, 0xfe000000, 0xfe000000, 0xfe000000
        .long   0xfe000000, 0xfe000000, 0xfe000000

| ---- Dedicated PULSE BD SRC page -------------------------------------------
| The eight values remain in SLICE's storage slots, so sounds and parameter
| locks keep working through Core 2.1.  Their presentation is wholly PULSE
| BD's: eight useful controls, ordinary round dials, and custom names.
        .equ    DP_ID,       8
        .equ    DP_LAST,     17

| Encoder D reaches the sample browser from the SRC encoder dispatcher,
| before the SRC page setter. At 0x4003b5b2, d2 is the selected parameter
| (0x87 for D), a2 is the SRC view, and a0 is the ordinary edit helper.
| Branch to the ordinary path only for machine 8 + parameter 0x87.
        .globl  dp_src_d_dispatch
dp_src_d_dispatch:
        cmpi.l  #0x87, %d2
        bne.s   1f
        lea     -16(%sp), %sp
        movem.l %d0-%d1/%a0-%a1, (%sp)
        move.l  %a2, -(%sp)
        jsr     0x4002b5d4
        addq.l  #4, %sp
        cmpi.l  #DP_ID, %d0
        blt.s   3f
        cmpi.l  #DP_LAST, %d0
        ble.s   2f
3:
        movem.l (%sp), %d0-%d1/%a0-%a1
        lea     16(%sp), %sp
        jmp     0x4003b5c0           | stock sample branch
2:      movem.l (%sp), %d0-%d1/%a0-%a1
        lea     16(%sp), %sp
        jmp     0x4003b5e0           | ordinary parameter branch
1:      jmp     0x4003b5b8           | original comparison's flags
        .equ    LAY_SLICE,   0x4197cf5c
        .equ    P_TUNE,      0x84
        .equ    P_PLAY,      0x85
        .equ    P_BR,        0x86
        .equ    P_SAMP,      0x87
        .equ    P_SLICE,     0x88
        .equ    P_LEN,       0x89
        .equ    P_GRID,      0x8a
        .equ    P_LEV,       0x8b

| 0x400657cc(machine): remember the page's machine and return a private copy
| of SLICE's complete eight-control layout for PULSE BD.
        .globl  dp_layout
dp_layout:
        move.l  4(%sp), %d0
        move.l  %d0, dp_page_m
        cmpi.l  #DP_ID, %d0
        blt.s   4f
        cmpi.l  #DP_LAST, %d0
        ble.s   1f
4:
        moveq   #3, %d1
        jmp     0x400657d2
1:      tst.l   dp_lay_ok
        bne.s   3f
        lea     LAY_SLICE, %a0
        lea     dp_lay, %a1
        moveq   #11, %d0
2:      move.l  (%a0)+, (%a1)+
        subq.l  #1, %d0
        bne.s   2b
        moveq   #1, %d0
        move.l  %d0, dp_lay_ok
3:      move.l  #dp_lay, %d0
        rts

| Return a custom label for each of P_TUNE..P_LEV while PULSE BD's page is
| active.  The stock functions are resumed for every other page.
        .globl  dp_lab_short, dp_lab_long
dp_lab_short:
        lea     dp_short_tab, %a0
        bsr.s   dp_lab_pick
        bne.s   9f
        move.l  8(%sp), %d1
        cmpi.l  #164, %d1
        jmp     0x4000fe94
9:      rts

dp_lab_long:
        lea     dp_long_tab, %a0
        bsr.s   dp_lab_pick
        bne.s   9f
        move.l  8(%sp), %d1
        cmpi.l  #164, %d1
        jmp     0x4000feb6
9:      rts

dp_lab_pick:
        move.l  dp_page_m, %d1
        subi.l  #DP_ID, %d1
        cmpi.l  #9, %d1
        bhi.s   8f
        mulu.w  #32, %d1               | 8 pointers per machine
        adda.l  %d1, %a0
        move.l  12(%sp), %d0
        subi.l  #P_TUNE, %d0
        cmpi.l  #7, %d0
        bhi.s   8f
        lsl.l   #2, %d0
        move.l  0(%a0,%d0.l), %d0
        rts
8:      moveq   #0, %d0
        rts

| d0 = parameter id -> d1 nonzero for one of our eight controls.
dp_is_control:
        cmpi.l  #P_TUNE, %d0
        bcs.s   8f
        cmpi.l  #P_LEV, %d0
        bhi.s   8f
        move.l  dp_page_m, %d1
        subi.l  #DP_ID, %d1
        cmpi.l  #9, %d1
        bhi.s   8f
        moveq   #1, %d1
        rts
8:      moveq   #0, %d1
        rts

| CHARACTER is the only non-numeric value: CLEAN, SWEEP, PUNCH or BOTH.
dp_is_character:
        cmpi.l  #P_PLAY, %d0
        bne.s   8f
        moveq   #DP_ID, %d1
        cmp.l   dp_page_m, %d1
        bne.s   8f
        moveq   #1, %d1
        rts
8:      moveq   #0, %d1
        rts

        .globl  dp_val_text
dp_val_text:
        move.l  8(%sp), %d0
        bsr.w   dp_is_character
        beq.s   1f
        move.l  12(%sp), -(%sp)
        move.l  20(%sp), -(%sp)
        jsr     dp_fmt_character
        addq.l  #8, %sp
        rts
1:      lea     -20(%sp), %sp
        movem.l %d2-%d4/%a2-%a3, (%sp)
        jmp     0x4000f32c

        .globl  dp_pop_text
dp_pop_text:
        move.l  4(%sp), %d0
        bsr.w   dp_is_character
        beq.s   1f
        move.l  8(%sp), -(%sp)
        pea     dp_txt
        jsr     dp_fmt_character
        addq.l  #8, %sp
        rts
1:      move.l  4(%sp), %d1
        cmpi.l  #164, %d1
        jmp     0x400657f8

| All controls except PITCH use BR's ordinary round-dial drawing.  Scale the
| four-position CHARACTER value across the dial instead of drawing it in the
| first three pixels of a 0..127 arc.
        .globl  dp_knob_gfx
dp_knob_gfx:
        move.l  8(%sp), %d0
        bsr.w   dp_is_control
        beq.s   2f
        cmpi.l  #P_TUNE, %d0
        beq.s   2f
        cmpi.l  #P_PLAY, %d0
        bne.s   1f
        move.l  dp_page_m, %d1
        cmpi.l  #DP_ID, %d1
        bne.s   1f
        move.l  12(%sp), %d0
        mulu.w  #42, %d0
        move.l  %d0, 12(%sp)
1:      move.l  #P_BR, %d0
        move.l  %d0, 8(%sp)
2:      lea     -20(%sp), %sp
        movem.l %d2-%d6, (%sp)
        jmp     0x4000f2c4

| Use BR's continuous-control UI record for every control except PITCH.  This
| removes SLICE's sample picker, bracket/count graphics and list icons.
        .globl  dp_ui_rec
dp_ui_rec:
        move.l  4(%sp), %d0
        bsr.w   dp_is_control
        beq.s   1f
        cmpi.l  #P_TUNE, %d0
        beq.s   1f
        move.l  #P_BR, %d1
        bra.s   2f
1:      move.l  4(%sp), %d1
2:      cmpi.l  #164, %d1
        jmp     0x4006579e

| A parameter range lookup reaches here from the page's setter, stepper,
| clamp and validator.  Restrict changes to the PULSE BD page object and give
| CHARACTER 0..3 and the six continuous macro controls 0..127.
        .globl  dp_prange, dp_prange_f
dp_prange:
        movea.l %a2, %a1
        bra.s   1f
dp_prange_f:
        movea.l 36(%sp), %a1
1:      move.l  %a1, -(%sp)
        move.l  %a0, -(%sp)
        move.l  12(%sp), -(%sp)
        jsr     0x40078f0c
        addq.l  #4, %sp
        movea.l (%sp)+, %a0
        movea.l (%sp)+, %a1
        move.l  4(%sp), %d1
        cmpi.l  #P_TUNE, %d1
        bcs.w   9f
        cmpi.l  #P_LEV, %d1
        bhi.w   9f
        move.l  (%a1), %d0
        cmpi.l  #0x4017eb58, %d0
        bne.w   9f
        movea.l 16(%a1), %a1
        move.l  (%a1), %d0
        cmpi.l  #0x40181330, %d0
        bne.w   9f
        movea.l 16(%a1), %a1
        moveq   #0, %d0
        move.b  126(%a1), %d0
        cmpi.l  #DP_ID, %d0
        blt.s   9f
        cmpi.l  #DP_LAST, %d0
        bgt.s   9f
        cmpi.l  #DP_ID, %d0
        bne.s   8f
        cmpi.l  #P_PLAY, %d1
        bcs.s   9f
        subi.l  #DP_ID, %d0
        mulu.w  #56, %d0               | 7 range pairs per machine
        subi.l  #P_PLAY, %d1
        lsl.l   #3, %d1
        add.l   %d0, %d1
        lea     dp_range_tab, %a1
        clr.l   (%a0)
        move.l  0(%a1,%d1.l), %d0
        move.l  %d0, 4(%a0)
        move.l  4(%a1,%d1.l), %d0
        move.l  %d0, 8(%a0)
        bra.s   9f
8:      subi.l  #9, %d0
        mulu.w  #8, %d0
        subi.l  #P_TUNE, %d1
        add.l   %d0, %d1
        lea     dd_trx_defaults_u7, %a1
        clr.l   (%a0)
        move.l  #0x7f00, %d0
        move.l  %d0, 4(%a0)
        moveq   #0, %d0
        move.b  0(%a1,%d1.l), %d0
        lsl.l   #8, %d0
        move.l  %d0, 8(%a0)
9:      move.l  %a0, %d0
        rts

        .balign 4
dp_short_tab:
        .long   dp_s_pitch, dp_s_char, dp_s_tone, dp_s_punch
        .long   dp_s_sweep, dp_s_decay, dp_s_drive, dp_s_level
        .long   dp_s_ptch, dp_s_dec, dp_s_ramp, dp_s_rdec
        .long   dp_s_strt, dp_s_nois, dp_s_harm, dp_s_clip
        .long   dp_s_ptch, dp_s_dec, dp_s_ramp, dp_s_hold
        .long   dp_s_tick, dp_s_nois, dp_s_dirt, dp_s_dist
        .long   dp_s_ptch, dp_s_dec, dp_s_bump, dp_s_benv
        .long   dp_s_snap, dp_s_tone, dp_s_tune, dp_s_clip
        .long   dp_s_gap, dp_s_dec, dp_s_hpf, dp_s_lpf
        .long   dp_s_mtal, dp_s_none, dp_s_none, dp_s_none
        .long   dp_s_gap, dp_s_dec, dp_s_hpf, dp_s_lpf
        .long   dp_s_mtal, dp_s_none, dp_s_none, dp_s_none
        .long   dp_s_rich, dp_s_dec, dp_s_top, dp_s_ttun
        .long   dp_s_size, dp_s_peak, dp_s_none, dp_s_none
        .long   dp_s_ptch, dp_s_dec, dp_s_dist, dp_s_none
        .long   dp_s_none, dp_s_none, dp_s_none, dp_s_none
        .long   dp_s_ptch, dp_s_dec, dp_s_enh, dp_s_tone
        .long   dp_s_bump, dp_s_none, dp_s_none, dp_s_dist
        .long   dp_s_ptch, dp_s_dec, dp_s_dual, dp_s_enh
        .long   dp_s_tune, dp_s_clic, dp_s_none, dp_s_none
dp_long_tab:
        .long   dp_l_pitch, dp_l_char, dp_l_tone, dp_l_punch
        .long   dp_l_sweep, dp_l_decay, dp_l_drive, dp_l_level
        .long   dp_l_ptch, dp_l_dec, dp_l_ramp, dp_l_rdec
        .long   dp_l_strt, dp_l_nois, dp_l_harm, dp_l_clip
        .long   dp_l_ptch, dp_l_dec, dp_l_ramp, dp_l_hold
        .long   dp_l_tick, dp_l_nois, dp_l_dirt, dp_l_dist
        .long   dp_l_ptch, dp_l_dec, dp_l_bump, dp_l_benv
        .long   dp_l_snap, dp_l_tone, dp_l_tune, dp_l_clip
        .long   dp_l_gap, dp_l_dec, dp_l_hpf, dp_l_lpf
        .long   dp_l_mtal, dp_l_none, dp_l_none, dp_l_none
        .long   dp_l_gap, dp_l_dec, dp_l_hpf, dp_l_lpf
        .long   dp_l_mtal, dp_l_none, dp_l_none, dp_l_none
        .long   dp_l_rich, dp_l_dec, dp_l_top, dp_l_ttun
        .long   dp_l_size, dp_l_peak, dp_l_none, dp_l_none
        .long   dp_l_ptch, dp_l_dec, dp_l_dist, dp_l_none
        .long   dp_l_none, dp_l_none, dp_l_none, dp_l_none
        .long   dp_l_ptch, dp_l_dec, dp_l_enh, dp_l_tone
        .long   dp_l_bump, dp_l_none, dp_l_none, dp_l_dist
        .long   dp_l_ptch, dp_l_dec, dp_l_dual, dp_l_enh
        .long   dp_l_tune, dp_l_clic, dp_l_none, dp_l_none
| {maximum, default}, 8.8 values, for PLAY through LEV.
dp_range_tab:
        .long   0x0300, 0x0300          | CHARACTER
        .long   0x7f00, 0x5000          | TONE
        .long   0x7f00, 0x2800          | PUNCH
        .long   0x7f00, 0x4000          | SWEEP
        .long   0x7f00, 0x4800          | DECAY
        .long   0x7f00, 0x2000          | DRIVE
        .long   0x7f00, 0x6400          | LEVEL

dp_s_pitch: .asciz "PITCH"
dp_s_char:  .asciz "CHAR"
dp_s_tone:  .asciz "TONE"
dp_s_punch: .asciz "PUNCH"
dp_s_sweep: .asciz "SWEEP"
dp_s_decay: .asciz "DECAY"
dp_s_drive: .asciz "DRIVE"
dp_s_level: .asciz "LEVEL"
dp_l_pitch: .asciz "Pitch"
dp_l_char:  .asciz "Character"
dp_l_tone:  .asciz "Tone"
dp_l_punch: .asciz "Punch"
dp_l_sweep: .asciz "Pitch Sweep"
dp_l_decay: .asciz "Decay"
dp_l_drive: .asciz "Drive"
dp_l_level: .asciz "Level"
dp_s_ptch:  .asciz "PTCH"
dp_s_dec:   .asciz "DEC"
dp_s_ramp:  .asciz "RAMP"
dp_s_rdec:  .asciz "RDEC"
dp_s_strt:  .asciz "STRT"
dp_s_nois:  .asciz "NOIS"
dp_s_harm:  .asciz "HARM"
dp_s_clip:  .asciz "CLIP"
dp_s_gap:   .asciz "GAP"
dp_s_hpf:   .asciz "HPF"
dp_s_lpf:   .asciz "LPF"
dp_s_mtal:  .asciz "MTAL"
dp_s_rich:  .asciz "RICH"
dp_s_top:   .asciz "TOP"
dp_s_ttun:  .asciz "TTUN"
dp_s_size:  .asciz "SIZE"
dp_s_peak:  .asciz "PEAK"
dp_s_enh:   .asciz "ENH"
dp_s_dual:  .asciz "DUAL"
dp_s_clic:  .asciz "CLIC"
dp_s_none:  .asciz "-"
dp_s_hold:  .asciz "HOLD"
dp_s_tick:  .asciz "TICK"
dp_s_dirt:  .asciz "DIRT"
dp_s_dist:  .asciz "DIST"
dp_s_bump:  .asciz "BUMP"
dp_s_benv:  .asciz "BENV"
dp_s_snap:  .asciz "SNAP"
dp_s_tune:  .asciz "TUNE"
dp_l_ptch:  .asciz "Pitch"
dp_l_dec:   .asciz "Decay"
dp_l_ramp:  .asciz "Pitch Ramp"
dp_l_rdec:  .asciz "Ramp Decay"
dp_l_strt:  .asciz "Start"
dp_l_nois:  .asciz "Noise"
dp_l_harm:  .asciz "Harmonics"
dp_l_clip:  .asciz "Clip"
dp_l_gap:   .asciz "Gap"
dp_l_hpf:   .asciz "Highpass"
dp_l_lpf:   .asciz "Lowpass"
dp_l_mtal:  .asciz "Metal"
dp_l_rich:  .asciz "Richness"
dp_l_top:   .asciz "Top"
dp_l_ttun:  .asciz "Top Tune"
dp_l_size:  .asciz "Size"
dp_l_peak:  .asciz "Peak"
dp_l_enh:   .asciz "Enhance"
dp_l_dual:  .asciz "Dual"
dp_l_clic:  .asciz "Click"
dp_l_none:  .asciz "Unused"
dp_l_hold:  .asciz "Hold"
dp_l_tick:  .asciz "Tick"
dp_l_dirt:  .asciz "Dirt"
dp_l_dist:  .asciz "Distortion"
dp_l_bump:  .asciz "Bump"
dp_l_benv:  .asciz "Bump Envelope"
dp_l_snap:  .asciz "Snap"
dp_l_tune:  .asciz "Detune"
        .balign 2
