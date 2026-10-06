# TRX-B2 characterization

Status: **reference characterized; new C synthesis model in regression**. This is a clean-room behavioral model, not firmware emulation or waveform identity. The 77-case Gearmulator sweep used a fresh Reaper process for each parameter group, empty bank E pattern 1, a 20-second boot wait, and a short throwaway render to force the plug-in's internal controls to synchronize with Reaper. The ten default-patch anchors all passed the consistency audit; no capture was rejected or tail-censored.

The original C TRX-B2 branch was discarded. The replacement in `machines/trx_md.c` uses the following measurements. Values are approximate because the 50 ms RMS windows include low-frequency phase effects; the first regression pass is documented separately.

| Control | Reference observation | New synthesis model |
| --- | --- | --- |
| PTCH | Late pitch rises from about 9 Hz at 0 to 107 Hz at 127. | Measured piecewise pitch-floor table; base patch 64 gives about 50 Hz. |
| RAMP | With base PTCH, pitch excess falls exponentially over about 42 ms. At 0 it is absent; at 127 its initial depth is about 430 Hz. | Exponential pitch sweep, depth proportional to RAMP. |
| DEC | Single-hit amplitude decays approximately exponentially. Body time constant spans roughly 10 ms to several seconds; the 127 setting reaches −40 dB near 19.7 s. | Measured piecewise decay constants, with a faster late release at the top end. |
| HOLD | The body decays slowly before DEC release. T40 moves from 340 ms at HOLD 0 to 9.9 s at 127. | Piecewise hold-duration table with a slow loss during the hold stage. |
| TICK | Peak rises from about 0.26 at 0 to 0.53 by 51, while 50 ms RMS barely changes. | Sub-millisecond impulse. |
| NOIS | At these settings, early RMS and spectrum change little across the sweep. | Low-level, brief noise transient. |
| DIRT | Early spectral centroid rises from about 108 to 219 Hz; RMS increases modestly. | Increasing bit reduction and modest gain. |
| DIST | Early RMS rises from about 0.14 to 0.20 and spectral centroid from about 110 to 249 Hz; the peak stays near 0.53. | Driven, clipped body before the decay envelope. |

The initial unprimed sweep was invalid for synthesis: Reaper's stored normalized value could equal the requested value while the freshly loaded kit still held a different internal value. A diagnostic sequence of PTCH 64 → 127 → 64 moved late pitch from about 10 Hz to about 50 Hz. The current capture script primes all controls before every group; the earlier 198-case trial and first 77-case trial are historical only.

The first 77-case regression pass is summarized in `REGRESSION.md`. Host tests and ColdFire linking pass. Audible similarity, retrigger behavior, hardware CPU cost, and Digitakt playback still need the user's assessment.
