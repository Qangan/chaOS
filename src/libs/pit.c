#include "pit.h"
#include "asm_utils.h"
#include "interrupts.h"
#include "vga.h"

u32 timer_ticks = 0;

void pit_init(u8 ch, u8 l, u8 h, u8 mode, u8 bcd, u32 hz) {
    u32 divisor = 1193182 / hz;
    u8 cfg = 0 << 7;
    cfg |= ch << 6;
    cfg |= l << 5;
    cfg |= h << 4;
    cfg |= mode << 1;
    cfg |= bcd;
    outb(PIT_CMD, cfg);
    outb(PIT_CH0, divisor & 0xFF);
    outb(PIT_CH0, (divisor >> 8) & 0xFF);
}

void timer_timer(context *ctx) {
    timer_ticks++;
    if (timer_ticks % 100 == 0)
        printf("%s\n", "one second");
}
