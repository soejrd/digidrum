CC ?= cc
CFLAGS ?= -std=c99 -O2 -Wall -Wextra -Werror
CROSS ?= m68k-elf-
CROSS_CFLAGS = -mcpu=54455 -O2 -ffreestanding -fno-builtin -nostdlib \
	-fno-pic -fno-pie -fomit-frame-pointer -Wall -Wextra -Werror
INCFLAGS = -I . -I include -I tools

DSP_SOURCES = dsp/tables.c dsp/osc.c dsp/envelope.c dsp/filter.c

.PHONY: all test demo cross-check cross-check-dsp clean web efm-test

all: test dsp-test

out:
	mkdir -p out

out/test_fixed: tests/test_fixed.c | out
	$(CC) $(CFLAGS) $(INCFLAGS) tests/test_fixed.c -o $@

out/test_tables: dsp/tables.c include/dd_tables.h tests/test_tables.c | out
	$(CC) $(CFLAGS) $(INCFLAGS) dsp/tables.c tests/test_tables.c -o $@

out/test_osc: dsp/osc.c dsp/tables.c include/dd_osc.h include/dd_tables.h tests/test_osc.c | out
	$(CC) $(CFLAGS) $(INCFLAGS) dsp/osc.c dsp/tables.c tests/test_osc.c -o $@

out/test_envelope: dsp/envelope.c include/dd_envelope.h tests/test_envelope.c | out
	$(CC) $(CFLAGS) $(INCFLAGS) dsp/envelope.c tests/test_envelope.c -o $@

out/test_noise: tests/test_noise.c | out
	$(CC) $(CFLAGS) $(INCFLAGS) tests/test_noise.c -o $@

out/test_filter: dsp/filter.c include/dd_filter.h tests/test_filter.c | out
	$(CC) $(CFLAGS) $(INCFLAGS) dsp/filter.c tests/test_filter.c -o $@

out/test_resonator: tests/test_resonator.c | out
	$(CC) $(CFLAGS) $(INCFLAGS) tests/test_resonator.c -o $@

out/test_rate: tests/test_rate.c | out
	$(CC) $(CFLAGS) $(INCFLAGS) tests/test_rate.c -o $@

out/test_param_cache: include/dd_param_cache.h include/dd_machine.h tests/test_param_cache.c | out
	$(CC) $(CFLAGS) $(INCFLAGS) tests/test_param_cache.c -o $@

out/test_benchmark_voice: tools/benchmark_voice.c tools/benchmark_voice.h include/dd_param_cache.h dsp/osc.c dsp/tables.c dsp/envelope.c tests/test_benchmark_voice.c | out
	$(CC) $(CFLAGS) $(INCFLAGS) tools/benchmark_voice.c dsp/osc.c dsp/tables.c dsp/envelope.c tests/test_benchmark_voice.c -o $@

out/test_trx_md: machines/trx_md.c include/dd_trx_md.h dsp/osc.c dsp/tables.c dsp/envelope.c tests/test_trx_md.c | out
	$(CC) $(CFLAGS) $(INCFLAGS) machines/trx_md.c dsp/osc.c dsp/tables.c dsp/envelope.c tests/test_trx_md.c -o $@

out/test_efm: machines/efm.c include/dd_efm.h dsp/osc.c dsp/tables.c tests/test_efm.c | out
	$(CC) $(CFLAGS) $(INCFLAGS) machines/efm.c dsp/osc.c dsp/tables.c tests/test_efm.c -o $@

efm-test: out/test_efm
	./out/test_efm

out/render_benchmark: tools/benchmark_voice.c tools/benchmark_voice.h include/dd_param_cache.h dsp/osc.c dsp/tables.c dsp/envelope.c tools/render_benchmark.c | out
	$(CC) $(CFLAGS) $(INCFLAGS) tools/benchmark_voice.c dsp/osc.c dsp/tables.c dsp/envelope.c tools/render_benchmark.c -o $@

out/render_trx_md: tools/render_trx_md.c machines/trx_md.c include/dd_trx_md.h dsp/osc.c dsp/tables.c dsp/envelope.c | out
	$(CC) $(CFLAGS) $(INCFLAGS) tools/render_trx_md.c machines/trx_md.c dsp/osc.c dsp/tables.c dsp/envelope.c -o $@

out/render_machine: tools/render_machine.c machines/trx_md.c include/dd_trx_md.h dsp/osc.c dsp/tables.c dsp/envelope.c | out
	$(CC) $(CFLAGS) $(INCFLAGS) tools/render_machine.c machines/trx_md.c dsp/osc.c dsp/tables.c dsp/envelope.c -o $@

test: out/test_fixed out/test_tables out/test_osc out/test_envelope out/test_noise out/test_filter out/test_resonator out/test_rate out/test_param_cache out/test_benchmark_voice out/test_trx_md out/test_efm
	./out/test_fixed
	./out/test_tables
	./out/test_osc
	./out/test_envelope
	./out/test_noise
	./out/test_filter
	./out/test_resonator
	./out/test_rate
	./out/test_param_cache
	./out/test_benchmark_voice
	./out/test_trx_md
	./out/test_efm
	python3 -m unittest tests/test_efm_perf_report.py

dsp-test: out/test_fixed out/test_tables out/test_osc out/test_envelope out/test_noise out/test_filter out/test_resonator out/test_rate out/test_param_cache out/test_benchmark_voice out/test_trx_md out/test_efm
	./out/test_fixed
	./out/test_tables
	./out/test_osc
	./out/test_envelope
	./out/test_noise
	./out/test_filter
	./out/test_resonator
	./out/test_rate
	./out/test_param_cache
	./out/test_benchmark_voice
	./out/test_trx_md
	./out/test_efm

demo: out/render_benchmark out/render_trx_md
	./out/render_benchmark
	./out/render_trx_md

WEB_EMCC ?= emsdk/upstream/emscripten/emcc

website/trx-synth.wasm: website/trx_web.c machines/trx_md.c machines/trx_family.c machines/efm.c include/dd_trx_md.h include/dd_trx_family.h include/dd_efm.h dsp/osc.c dsp/tables.c dsp/envelope.c
	EM_CONFIG="$(CURDIR)/emsdk/.emscripten" $(WEB_EMCC) -O2 -std=c99 -I include \
		-s STANDALONE_WASM=1 -Wl,--no-entry -Wl,--export-memory \
		-Wl,--export=dd_web_init -Wl,--export=dd_web_set_control \
		-Wl,--export=dd_web_set_level -Wl,--export=dd_web_trigger \
		-Wl,--export=dd_web_render -Wl,--export=dd_web_capacity \
		-Wl,--export=dd_web_machine_count -Wl,--export=dd_web_control_count \
		-Wl,--export=dd_web_default_control \
		website/trx_web.c machines/trx_md.c machines/trx_family.c machines/efm.c dsp/osc.c dsp/tables.c dsp/envelope.c \
		-o $@

web: website/trx-synth.wasm

cross-check: | out
	mkdir -p out/cross
	$(CROSS)as -mcpu=54455 -I . -o out/cross/glue.o glue.s
	$(CROSS)gcc $(CROSS_CFLAGS) -I . -c digitakt.c -o out/cross/digitakt.o
	$(CROSS)gcc $(CROSS_CFLAGS) $(INCFLAGS) -c machines/trx_md.c -o out/cross/trx_md.o
	$(CROSS)gcc $(CROSS_CFLAGS) $(INCFLAGS) -c machines/trx_family.c -o out/cross/trx_family.o
	$(CROSS)gcc $(CROSS_CFLAGS) $(INCFLAGS) -c machines/efm.c -o out/cross/efm.o
	$(CROSS)gcc $(CROSS_CFLAGS) $(INCFLAGS) -c dsp/osc.c -o out/cross/osc.o
	$(CROSS)gcc $(CROSS_CFLAGS) $(INCFLAGS) -c dsp/tables.c -o out/cross/tables.o
	$(CROSS)gcc $(CROSS_CFLAGS) $(INCFLAGS) -c dsp/envelope.c -o out/cross/envelope.o
	$(CROSS)ld -r -d -T tools/elekloader-mod.ld -o out/cross/digidrum.o \
		out/cross/glue.o out/cross/digitakt.o \
		out/cross/trx_md.o out/cross/trx_family.o out/cross/efm.o out/cross/osc.o out/cross/tables.o out/cross/envelope.o
	@! $(CROSS)nm -u out/cross/digidrum.o | grep .
	$(CROSS)size -A out/cross/digidrum.o

cross-check-dsp: | out
	mkdir -p out/cross
	$(CROSS)gcc $(CROSS_CFLAGS) $(INCFLAGS) -c dsp/tables.c -o out/cross/tables.o
	$(CROSS)gcc $(CROSS_CFLAGS) $(INCFLAGS) -c dsp/osc.c -o out/cross/osc.o
	$(CROSS)gcc $(CROSS_CFLAGS) $(INCFLAGS) -c dsp/envelope.c -o out/cross/envelope.o
	$(CROSS)gcc $(CROSS_CFLAGS) $(INCFLAGS) -c dsp/filter.c -o out/cross/filter.o
	$(CROSS)gcc $(CROSS_CFLAGS) $(INCFLAGS) -c tools/benchmark_voice.c -o out/cross/benchmark_voice.o
	$(CROSS)gcc $(CROSS_CFLAGS) $(INCFLAGS) -c machines/trx_md.c -o out/cross/trx_md.o
	$(CROSS)gcc $(CROSS_CFLAGS) $(INCFLAGS) -c machines/trx_family.c -o out/cross/trx_family.o
	$(CROSS)gcc $(CROSS_CFLAGS) $(INCFLAGS) -c machines/efm.c -o out/cross/efm.o
	$(CROSS)size -A out/cross/tables.o out/cross/osc.o out/cross/envelope.o out/cross/filter.o out/cross/benchmark_voice.o out/cross/trx_md.o out/cross/trx_family.o out/cross/efm.o

clean:
	rm -f out/test_* out/render_benchmark out/render_trx_md out/render_machine out/benchmark-voice.wav out/trx-b2.wav out/trx-sd.wav
	rm -rf out/cross
