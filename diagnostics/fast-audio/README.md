# Digidrum FAST AUDIO diagnostic

This is an adapted copy of the GPL-2.0-or-later Digihealth mod from
`digisophie/diagnostics/digihealth`, used with Digitakt Mk1 OS 1.53. It adds
FAST AUDIO and SYSTEM INFO to the combined Digidrum build. Its own mod ID is
`digidrum-health`; it conflicts with the original `digihealth` mod.

Digidrum changes are small: FAST AUDIO is enabled automatically about two
seconds after the UI starts, and SYSTEM INFO is enabled on the first UI tick.
FAST AUDIO copies stock render code to unused on-chip SRAM, with its existing
refusal and watchdog checks. It is a speed optimization, not a removal of the
sample engine. The Settings row can still turn it off for the current boot.

Build with the owner-supplied stock image and Elekloader on `PYTHONPATH`:

```sh
cd digidrum/diagnostics/fast-audio
ELEKLOADER_CROSS=m68k-elf- PYTHONPATH=../../../elekloader \
  python3 build.py --stock ../../../firmware/Digitakt_OS1.53_dist/Digitakt_OS1.53.syx
```

The SYSTEM INFO CPU/DSP calculation uses hardware DMA timer 0. The local
Digiemu now advances that counter with guest time, so the UI shows emulator
estimates. Its step-boundary timing is coarse and does not replace the user's
hardware measurements.
