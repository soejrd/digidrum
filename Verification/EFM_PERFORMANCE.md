# EFM performance measurements on Digitakt Mk1

The two Digitakt Mk1 OS 1.53 images are in `out/`:

- `Digitakt_OS1.53_DIGIDRUM_EFM_HEALTH.syx` (`E1DH`): EFM machines and the SYSTEM INFO/FAST AUDIO diagnostic, without per-machine timing.
- `Digitakt_OS1.53_DIGIDRUM_EFM_PERF.syx` (`E1PF`): the same machines plus per-machine ColdFire timing counters. Use this one for the matrix.

Both were built from the owner-supplied stock OS 1.53 with elekloader Core 2.1. The packer verified the unchanged non-OS sections and the modified main OS. These checks establish packaging, not hardware stability or available audio headroom.

## What the probe measures

`digitakt.c` reads the same DMA timer 0 counter as digihealth immediately before and after each EFM voice's 32-sample `dd_efm_render` call. It subtracts the minimum adjacent-read cost measured during the first 128 audio blocks. Each EFM kind has four counters:

1. **Idle:** no active voice and no changed synth control.
2. **Steady:** an active voice with no trigger or changed control.
3. **Trigger:** a hit starts this block.
4. **Control:** a synth control changed without a trigger.

Each counter records a recent 512–1024-call average and a recent minimum/maximum. Hardware time includes any interrupt that lands inside the measured call. The reported peak is therefore an observed worst block, not a guaranteed worst-case execution time. The table does not include parameter reading, the stock effects path, or mixing; digihealth's whole-render figures cover those together.

## Capture a matrix

Install the `E1PF` image on the Digitakt. Connect it over USB MIDI. The report tool uses digihealth's read-only USB channel; live mode needs `mido` and `python-rtmidi` installed in the Python environment. Find the exact port name with:

```sh
python3 -c 'import mido; print(mido.get_input_names()); print(mido.get_output_names())'
```

For each EFM machine, run a repeating pattern at its default controls, wait at least a second for the steady window to fill, then run:

```sh
python3 tools/efm_perf_report.py --port 'Digitakt MIDI port name'
```

The output is a Markdown matrix with average EFM render ticks, each machine's share of a 32-sample audio period, trigger peak, and digihealth whole-render CPU/DSP figures. If the input and output MIDI port names differ, add `--input-port 'input name'`. A saved raw 640-byte PEEK dump can be decoded with `--raw dump.bin`; a saved PEEK SysEx reply can be decoded with `--reply reply.syx`.

Capture the following scenarios with FAST AUDIO **off**, then repeat the worst one with it **on**. This build enables FAST AUDIO automatically after startup; switch it off in SETTINGS before taking the first set.

| Scenario | What it answers |
| --- | --- |
| Stock sample or silent pattern | Whole-render baseline |
| One EFM track, repeated triggers | Per-machine steady and trigger cost |
| Four EFM tracks, simultaneous triggers | Scaling and cache effects |
| Eight EFM tracks, simultaneous triggers | Practical peak and audio stability |
| Eight tracks with parameter locks | Control update spikes |

Record audible glitches and UI responsiveness alongside the numbers. The live report's timer ticks are calibrated as a fraction of the measured audio-block period; no assumed ColdFire clock frequency is needed. Compare the `E1PF` and `E1DH` whole-render load with the same pattern if probe overhead itself becomes significant.

Reboot between the one-, four-, and eight-track cases if you want independent trigger averages and peaks. Otherwise those counters include earlier hits of the same machine; steady averages adapt to the recent window.

The matrix has no measured hardware values until a Digitakt runs these cases. Browser, host and emulator timing cannot populate it reliably.
