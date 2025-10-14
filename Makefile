# =============================================================================
# Variables

# =============================================================================
# Tasks

SRCLIBS := $(wildcard src/libs/*.c)
OBJLIBS := $(patsubst src/libs/%.c,.tmp/%.o,$(filter-out src/libs/kpanic.c,$(SRCLIBS)))
KPANIC := .tmp/kpanic.o

all: clean build test

$(KPANIC): src/libs/kpanic.c | .tmp
	gcc -std=c99 -m32 -O2 -ffreestanding -no-pie -fno-pie -mno-sse -fno-stack-protector -masm=intel -Iinclude -c $< -o $@

OBJLIBS += $(KPANIC)

.tmp/%.o: src/libs/%.c | .tmp
	gcc -std=c99 -m32 -O2 -ffreestanding -no-pie -fno-pie -mno-sse -fno-stack-protector -Iinclude -c $< -o $@

boot.img: $(OBJLIBS)
	gcc -std=c99 -m32 -O2 -ffreestanding -no-pie -fno-pie -mno-sse -fno-stack-protector -c src/kernel.c -o .tmp/kernel.o
	nasm -felf src/boot.asm -o .tmp/boot.o -dN=100000
	ld -m elf_i386 .tmp/boot.o .tmp/kernel.o $(OBJLIBS) -T link.ld -o .tmp/os.elf
	objcopy -I elf32-i386 -O binary .tmp/os.elf .tmp/os.bin
	dd if=/dev/zero of=boot.img bs=1024 count=1440
	dd if=.tmp/os.bin of=boot.img conv=notrunc

build: boot.img

clean:
	rm -f *.img
	rm -rf .tmp
	mkdir .tmp

test: build
	qemu-system-i386 -D ./log.txt -cpu pentium2 -m 4g -fda boot.img -monitor stdio -device VGA

debug: build
	qemu-system-i386 -D ./log.txt -cpu pentium2 -m 4g -fda boot.img -monitor stdio -device VGA -s -S &
	gdb

.PHONY: all build clean test debug
