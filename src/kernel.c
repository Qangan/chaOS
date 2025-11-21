#include "libs/interrupts.h"
#include "libs/kboard.h"
#include "libs/kpanic.h"
#include "libs/memory.h"
#include "libs/pit.h"
#include "libs/vga.h"

extern void loop();

u32 global = 0;

static void delay() {
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 50000; j++) {
            outb(0x80, 0x80);
        }
        printf("%d ", i);
    }
    printf("\n");
}

void kboard_handler(context *ctx) {
    printf("%x ", inb(0x60));
    // send_eoi(KBOARD);
    // sti();
    // loop();
}

void timer_handler(context *ctx) {
    printf("%d ", global++);
    // global = 0;
    // send_eoi(TIMER);
    // sti();
    // loop();
}

void kernel_entry() {
    clear_screen();
    init(INTERRUPT_GATE);
    pic_init(1);
    ps2_init();
    // pit_init(0, 1, 1, 3, 0, 100);
    setup_handler(TIMER, TVECTOR, timer_handler);
    setup_handler(KBOARD, KVECTOR, keyboard_handler);
    // setup_handler(KBOARD, KVECTOR, kboard_handler);
    // setup_handler(TIMER, TVECTOR, timer_handler);
    /*
    set_mask(TIMER);
    delay();
    sti();
    delay();
    sti();
    */
    sti();
    // for (;;) {printf("%d ", global++);};
    loop();
}
