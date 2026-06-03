#include "kpanic.h"
#include "vga.h"

void vkernel_panic(const char *fmt, va_list args) {
    u32 eax, ebx, ecx, edx, esi, edi, ebp, esp;
    u32 eflags;
    u16 cs, ds, es, ss, fs, gs;
    cli();
    clear_screen();
    change_bg_color(white);
    set_fg(black);
    printfln("%s", "Kernel panic :(");
    vprintf(fmt, args);
    loop();
}
void kernel_panic(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vkernel_panic(fmt, args);
    va_end(args);
}
