#include "libs/kpanic.h"
#include "libs/memory.h"
#include "libs/vga.h"
#include "libs/interrupts.h"

extern void loop();

void kernel_entry() {
    clear_screen();
    init();
        __asm__ volatile(   \
        "mov eax, 0x0\n"  \
        "mov ecx, 0xBBBBBBBB\n"  \
        "mov edx, 0xCCCCCCCC\n"  \
        "mov ebx, 0xDDDDDDDD\n"  \
        "mov esi, 0xEEEEEEEE\n"  \
        "mov edi, 0xFFFFFFFF\n"  \
        );
    //__asm__ volatile("int 0");
    //__asm__ volatile("int 2");
    //__asm__ volatile("idiv eax");
    sti();
}
