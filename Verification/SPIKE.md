# OS 1.53 sample-free machine spike

Status: **PULSE BD audio, SRC D editing, a D lock, and two-track simultaneous
trigs verified in Digiemu; disk project reload and hardware performance remain
open** (2026-10-05). This is a
focused experiment on the original Digitakt. Do not flash this build on the
basis of emulator results alone.

## Inputs and provenance

- Owner-supplied `Digitakt_OS1.53.syx` SHA-256:
  `9bdd44bb6102fb25c143cfab97bc92b7a89c463f795d3112dce89771e29bcc92`.
- Local elekloader `91840b1f`, Core 2.1; Digiemu `32064026` with an unrelated
  local edit to `emu/gui.py` left untouched; DigiSophie `27736f59`.
- `m68k-elf-gcc` 16.2.0. The SDK was run with
  `ELEKLOADER_CROSS=m68k-elf-` and `PYTHONPATH=../elekloader`.
- This prototype adapts the existing `digiperc` PULSE BD source and fixed-point
  voice. Its Mutable Instruments topology attribution is in
  [THIRD_PARTY.md](THIRD_PARTY.md). The prototype uses machine ID **8** so it
  cannot silently reinterpret earlier custom-machine sounds.

## Path map and evidence

**Code** means checked against the local Core 2.1 source and/or the exact
OS image. **Emulator** means traced or observed with the Digidrum D003 build.
The untested parts are marked explicitly.

| Path | Finding | Evidence |
| --- | --- | --- |
| Machine choice | Core's `core_machines` descriptor has ID, name, icon, stock parameter machine, and render machine. Its hooks at `0x40022fe6`, `0x400225f0`, `0x4002a4e0`, and `0x4002a9e8` add and select machines. The ID is stored at sound offset `+0x7e`; `core_mload` at `0x4007a2ea` preserves a registered added ID. | Core source and stock bytes; Digidrum menu untested. |
| Eight SRC slots | Machine 8 borrows SLICE's `0x84`–`0x8b` parameter IDs. The eight values are in the existing sound parameter storage. The page layout hook at `0x400657cc` copies SLICE's eight-control layout. Labels, values, dial types and ranges use the patch sites listed below. | Core, `glue.s`, stock bytes; editing untested. |
| Encoder D / sample action | SRC slot D is `0x87`. The encoder dispatcher at `0x4003b5b2` chooses the browser **before** the page setter. `dp_src_d_dispatch` checks the page machine and sends machine 8 to the ordinary edit branch at `0x4003b5e0`; other machines retain the stock branch at `0x4003b5c0`. The page vtable pointer at `0x4018492c` also routes machine 8's D setter to the ordinary setter. | Stock disassembly; D002 emulator trace reproduced the browser. D003 verified live editing and lock recall without the browser; disk persistence remains unverified. |
| Parameter locks | Core maps an added machine to its stock parameter descriptor at `0x40078f72`. Digidrum retains the same slot IDs and calls the ordinary setter for D. | Core source; in Digiemu, holding trig 1 and turning D showed lock value 38, release restored the base value 37, and reholding the trig recalled 38. Other slots and disk persistence remain unverified. |
| Save and reload | Core's load hook preserves the added ID in the sound at `0x4007a2ea`; the parameter fields remain stock fields. Disk project save/reload and sound pool recall have **not** been traced or tested for this mod. | Core source only for ID preservation. |
| Audio | Core's `core_mrender` at `0x40077272` puts render ID 8 into the track machine byte and leaves an empty stock voice window. `dp_inject_s` at `0x40077fba`, after stock playback and before overdrive, fills each active track's 32-sample buffer from `dp_voice_render`. Stock AMP, filter, mixer and sends follow. | Core and Digidrum source; the user confirmed audible PULSE BD output in Digiemu. Multiple simultaneous trigs remain untested. |

## Narrow patch

`dp_src_d_dispatch` patches the SRC encoder decision at `0x4003b5b2`. It
routes only machine 8, parameter `0x87`, to the ordinary edit branch. The
other parameters and machines keep their stock paths. `dp_pageset` also
replaces the SRC page setter vtable pointer at `0x4018492c`: machine 8's D
value uses the ordinary setter sequence from `0x40030a28`. The earlier
attempt to clear `core_samp_tab[8]` did not prevent the browser and was
removed. D002 tracing found the stock browser call; D003 tracing found no
sample lookup or browser call when turning D.

The rest of the patch provides the machine and its SRC page: label sites
`0x4000fe8a`, `0x4000feac`; value/dial sites `0x4000f324`, `0x4000f2bc`;
page UI sites `0x40065794`, `0x400657ee`; range sites `0x4000ff20`,
`0x400100c4`, `0x4000f534`, `0x4000f5fc`; layout `0x400657cc`; audio
`0x40077fba`; the D dispatcher adds `0x4003b5b2`. Core is required. Layout, presentation and audio sites conflict
with the current `digiperc`, `digisophie`, and `digineighbor` mods; the
manifest names those conflicts. Core's hooks are shared rather than
overwritten. Normal OS update and bootloader sections are not modified.

## SRC controls

| Encoder | Name | Existing slot | Intended range |
| --- | --- | --- | --- |
| A | PITCH | TUNE | stock pitch |
| B | CHAR | PLAY | 0–3 |
| C | TONE | BR | 0–127 |
| D | PUNCH | SAMP | 0–127 |
| E | SWEEP | SLICE | 0–127 |
| F | DECAY | LEN | 0–127 |
| G | DRIVE | GRID | 0–127 |
| H | LEVEL | LEV | 0–127 |

## Tests and limits

| Check | Result |
| --- | --- |
| Host DSP `make test` | Pass: deterministic early/late energy check. |
| Host eight-voice overlap | Pass: eight PULSE BD voices struck in the same block produced independent bounded output for 64 blocks. This does not establish emulator sequencer behavior or hardware headroom. |
| ColdFire compile/link `make cross-check` | Pass; Digidrum `.run` 3212 bytes, `.bss` 352 bytes, no unresolved imports. |
| SDK build and lint against exact stock, Core 2.1 and Digidrum diagnostics | Pass; Digidrum has 14 patch sites, no overlaps; combined linked RAM 10644 bytes with 120428 bytes spare in the mod region. |
| Firmware repack | Pass; unchanged sections 2, 4, 5 and 8 verified byte for byte. |
| Digiemu first boot and panel | Pass after reinstalling Digiemu's pinned patched Unicorn runtime. D001 booted; combined D003 booted and entered the panel. |
| PULSE BD sound | The user confirmed audible output in Digiemu. |
| SRC D browser and editing | D002 reproduced the stock browser. D003 hit `dp_src_d_dispatch` and the ordinary edit branch, with zero hits at the sample branch, sample lookup and browser calls. Five separate encoder detents changed track 1's D voice value from 37 to 39; the value survived a FILTER-to-SRC page round trip. |
| SRC D parameter lock | In Digiemu grid recording, holding trig 1 and turning D showed 38; release showed the base 37; reholding trig 1 showed 38 again. |
| Two-track simultaneous trigs | In an isolated Digiemu session, both tracks selected PULSE BD. Pattern playback set both render machine bytes to 8, and the `dp_voice_render` hook recorded trigger=1 for tracks 1 and 2 in the same audio block. PCM output continued and the session did not halt. |
| Stock SRC D regression | On a stock machine in the D003 build, the stock sample branch and browser call each ran once; the Digidrum dispatch hook was not entered. |
| FAST AUDIO | The integrated diagnostic auto-enables it after two seconds. D003's snapshot reports `fast=1`, `r_on=1`, `r_fault=0`; SYSTEM INFO overlay is enabled. |
| CPU/DSP readout in Digiemu | After importing the `panel-ui` branch's DTIM0 counter model, the D003 overlay showed numeric estimates (for one probe: CPU 15%, DSP 3%/55%). Digiemu advances the counter at step boundaries; these values are not hardware performance measurements. |
| Disk project save/reload | Not yet verified. On an isolated card copy, the project menu was reachable and showed `SAVE PROJECT AS`; confirming `CREATE NEW` showed `SAVE NEW PROJECT BEFORE LOADING?`. That sequence produced no card overlay writes. A pattern shortcut probe changed both C and D and did not restore either live value, so it does not establish D-specific persistence behavior. |
| Hardware CPU/audio | Not tested. No eight-track performance claim. |

The combined D003 firmware is
`out/Digitakt_OS1.53_DIGIDRUM_HEALTH_D.syx` (SHA-256
`69c2ab903060d3c2c1145f8a279d7a65580871b72c49372c3411d488520d5165`).
The next smallest step is to verify disk project save and reload through a
completed save workflow. Map FILTER and AMP page
descriptors and storage before changing either page. Trace the stock sample
render path to identify actual runtime cost before removing shared sample
services. Hiding a browser saves UI complexity but does not by itself free
audio CPU or DSP capacity.
