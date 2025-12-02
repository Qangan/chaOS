#include "ps2.h"
#include "vga.h"

void ps2_init() {
    outb(KB_CMD, 0xAD);
    io_wait();
    outb(KB_CMD, 0xA7);
    if (inb(KB_STATUS) & 1)
        inb(KB_DATA);
    outb(KB_CMD, 0x20);
    u8 cfg = inb(KB_DATA);
    cfg &= ~1;
    cfg &= ~(1 << 1);
    cfg &= ~(1 << 6);
    outb(KB_CMD, 0x60);
    outb(KB_DATA, cfg);
    outb(KB_CMD, 0xAA);
    if (inb(KB_DATA) != 0x55)
        kernel_panic("%s", "PS/2 IS DEAD");
    outb(KB_CMD, 0xAB);
    if (inb(KB_DATA) != 0)
        kernel_panic("%s", "FIRST IS DEAD");
    outb(KB_CMD, 0xAE);
    cfg |= 0b01000001; // translation and first ps/2
    outb(KB_CMD, 0x60);
    outb(KB_DATA, cfg);
    outb(KB_DATA, 0xFF);
    if (inb(KB_DATA) != 0xFA)
        kernel_panic("%s", "NO KEYBOARD");
}
