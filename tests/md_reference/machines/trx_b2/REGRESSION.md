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
