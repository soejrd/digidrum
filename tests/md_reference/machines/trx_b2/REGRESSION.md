# TRX-B2 regression pass 1

The 77 clean, primed Gearmulator references were compared with host renders of the same `machines/trx_md.c` source used by the TRX3 emulator image. Both sides used the manifest's control values and matching 2–44 second capture windows. Generated per-case details are in the local `measurements/reference.json`, `measurements/candidate.json`, and `report/comparison.json` files.

| Case | Reference | Candidate |
| --- | ---: | ---: |
| Baseline peak | 0.527 | 0.528 |
| Baseline −40 dB time | 340 ms | 340 ms |
| Baseline 20 ms spectral centroid | 108 Hz | 108 Hz |
| DEC 127 −40 dB time | 19.66 s | 19.48 s |
| HOLD 127 −40 dB time | 9.87 s | 9.82 s |
| DEC 127 × HOLD 127 −40 dB time | 26.57 s | 26.80 s |
| DIRT 127 centroid | 219 Hz | 217 Hz |
| DIST 127 centroid | 249 Hz | 249 Hz |

Across the six values of each individual control, maximum peak error is 0.017 (NOIS), maximum early RMS error is 0.009 (DEC), maximum −40 dB time error is 180 ms (DEC), and maximum centroid error is 11 Hz (DIRT). Median absolute peak error across all 77 cases is 0.0027; median −40 dB time error is 0 ms at the report's 10 ms resolution.

The first pass exposed and fixed a pre-trigger TICK impulse, loss of long tails from Q15 envelope truncation, and a ColdFire link failure from 64-bit multiplication. The final envelope uses a Q24 state and exact split 32-bit multiplication. Host tests, ColdFire linking, mod lint, and SysEx patch verification pass.

These compact features do not prove that the waveform or sound is the same. The very low DEC cases still differ in residual low-frequency spectrum (reference centroid roughly 9–11 Hz, candidate 3–5 Hz), and NOIS peak response is less exact. User listening in Digiemu should determine whether attack character, pitch movement, tail, or driven tone needs the next focused capture or model revision.

## Pass 2: TRX4 listening corrections

User listening found a metallic tail and NOIS that was too dull and short.
The B2 body had been repeated at 48 kHz from a 24 kHz oscillator, creating a
high-frequency image throughout long decays. TRX4 linearly interpolates the
body. The DIRT quantizer had also rounded negative samples toward negative
infinity; it now rounds symmetrically around zero.

Three targeted DEC 127 / NOIS 0, 64, 127 Gearmulator captures showed a bright
broadband burst decaying for roughly 50–70 ms. TRX3 generated a faint burst for
only 10 ms. TRX4 generates shaped noise at the full 48 kHz output rate with a
roughly 9 ms exponential time constant. At NOIS 127, a 1024-sample Hann FFT
starting at the trigger gives 2–8 kHz energy 0.04921 reference / 0.04886
candidate and 8–24 kHz energy 0.04797 / 0.04318. At 40 ms the corresponding
figures are 0.00054 / 0.00050 and 0.00055 / 0.00053.

All 77 host cases rendered and measured after these changes. The baseline
−40 dB time remains 340 ms on both sides; DEC 127 is 19.66 s / 19.46 s,
HOLD 127 is 9.87 s / 9.82 s. The coarse spectrum still differs in driven
cases: DIRT 127 centroid is 219 / 210 Hz and DIST 127 is 249 / 223 Hz.
Those tone differences and short DEC residuals remain open. Host tests,
ColdFire link, mod lint, and SysEx patch verification pass. Live Digiemu audio
has not been verified in this environment.

## Pass 3: TRX5 tail and DIRT response

User listening confirmed the TRX4 NOIS improvement, but still heard a metallic
tail and an abrupt DIRT onset around 51. The clean B2 oscillator and envelope
now retain 31-bit output precision instead of rounding each body sample to
16 bits. The pitch sweep and amplitude decay are interpolated between their
control updates; DIRT crossfades between neighboring quantization depths.
The direct host renderer now writes 24-bit PCM so its measurements retain that
extra precision.

At 200 ms in the baseline, the candidate's normalized 2–8 kHz spectral energy
fell from 0.000006 to 0.000001. Gearmulator was below the report's six-decimal
resolution at that point. All 77 cases still render and compare. Baseline
−40 dB time remains 340 ms on both sides; DEC 127 is 19.66 s / 19.49 s and
HOLD 127 is 9.87 s / 9.82 s. The very quiet baseline tail after 500 ms still
differs, and a listening check is needed to determine whether the audible
metallic quality is gone. Host tests, ColdFire linking, mod lint, and SysEx
patch verification pass. Hardware audio and CPU headroom remain unverified.
