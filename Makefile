# =============================================================================
# Variables

# =============================================================================
# Tasks

CFLAGS := -std=c23 -m32 -O2 -Wimplicit-function-declaration -masm=intel -ffreestanding -no-pie -fno-pie -mno-sse -fno-stack-protector
LIBS := $(wildcard ./src/libs/*.c) 
KERNEL := $(wildcard ./src/*.c)

OBJLIBS := $(patsubst ./src/libs/%.c, .tmp/%.o, $(LIBS))
OBJKERNEL := $(patsubst ./src/%.c, .tmp/%.o, $(KERNEL))

ifdef ASSERT
    CFLAGS += -D DEBUG
endif

all: clean build test

.tmp/%.o: ./src/libs/%.c | .tmp
	gcc $(CFLAGS) -Iinclude -c $< -o $@

.tmp/%.o: ./src/%.c | .tmp
	gcc $(CFLAGS) -Iinclude -c $< -o $@

boot.img: $(OBJLIBS) $(OBJKERNEL)
	nasm -felf src/boot.asm -o .tmp/boot.o -dN=100000
	ld -m elf_i386 .tmp/boot.o $(OBJKERNEL) $(OBJLIBS) -T link.ld -o .tmp/os.elf
	objcopy -I elf32-i386 -O binary .tmp/os.elf .tmp/os.bin
	dd if=/dev/zero of=boot.img bs=1024 count=1440
	dd if=.tmp/os.bin of=boot.img conv=notrunc

build: boot.img

clean:
	rm -f *.img
	rm -rf .tmp
	mkdir .tmp

test: build
	qemu-system-i386 -D ./log.txt -cpu pentium2 -m 4m -fda boot.img -monitor stdio -device VGA

debug: clean build
	qemu-system-i386 -D ./log.txt -no-reboot -cpu pentium2 -m 4g -fda boot.img -monitor stdio -device VGA -s -S &

	gdb

.PHONY: all build clean test debug
