CC ?= cc
CFLAGS ?= -std=c99 -O2 -Wall -Wextra -Werror
CROSS ?= m68k-elf-
CROSS_CFLAGS = -mcpu=54455 -O2 -ffreestanding -fno-builtin -nostdlib \
	-fno-pic -fno-pie -fomit-frame-pointer -Wall -Wextra -Werror

.PHONY: all test demo cross-check clean

all: test

out:
	mkdir -p out

out/test_percussion: percussion.c percussion.h tests/test_percussion.c | out
	$(CC) $(CFLAGS) percussion.c tests/test_percussion.c -o $@

out/render_demo: percussion.c percussion.h tools/render_demo.c | out
	$(CC) $(CFLAGS) percussion.c tools/render_demo.c -o $@

test: out/test_percussion
	./out/test_percussion

demo: out/render_demo
	./out/render_demo

cross-check: | out
	mkdir -p out/cross
	$(CROSS)as -mcpu=54455 -I . -o out/cross/glue.o glue.s
	$(CROSS)gcc $(CROSS_CFLAGS) -I . -c digitakt.c -o out/cross/digitakt.o
	$(CROSS)gcc $(CROSS_CFLAGS) -I . -c percussion.c -o out/cross/percussion.o
	$(CROSS)ld -r -d -T tools/elekloader-mod.ld -o out/cross/digidrum.o \
		out/cross/glue.o out/cross/digitakt.o out/cross/percussion.o
	@! $(CROSS)nm -u out/cross/digidrum.o | grep .
	$(CROSS)size -A out/cross/digidrum.o

clean:
	rm -f out/test_percussion out/render_demo out/pulse-bd.wav
	rm -rf out/cross