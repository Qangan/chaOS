#include "libs/interrupts.h"
#include "libs/kboard.h"
#include "libs/kpanic.h"
#include "libs/memory.h"
#include "libs/pit.h"
#include "libs/vga.h"
#include "libs/userspace.h"

extern void loop();
extern void syscallprint(u32 n);


u32 global = 1;

void kboard_handler(context *ctx) {
    printf("%x ", inb(0x60));
    // send_eoi(KBOARD);
    // sti();
    // loop();
}

void timer_handler(context *ctx) {
    //printf("%x ", get_esp());
    //loop();
    //global = 0;
    // send_eoi(TIMER);
    // sti();
    // loop();
}

void printfloop(){
    printf("WINNERS KEEP WINNING!");
    loop();
}

void globalprint(){
    for (;;) { printf("%d ", ++global); } 
}

void printesp(){
    printf("%x", get_esp());
    loop();
}

void ustroydestroy() {
    extern u64 kcodesegment;
    kcodesegment &= ~(1 << 47); 
}

void syscall(context* ctx){
    printf("%u ", ctx->eax);
}

void usersyscall(){
    for (;;) {
        syscallprint(global++);
    }
}

void kernel_entry() {
    clear_screen();
    init(INTERRUPT_GATE);
    pic_init(1);
    setup_handler(TIMER, TVECTOR, timer_handler);
    setup_handler_no_irq(0x30, syscall);
    sti();
    process(usersyscall, immortal_alloc(4096, 16) + 4096);
}
