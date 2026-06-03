#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "memory.h"
#include "kpanic.h"
#include "asm_utils.h"


#define TRAP_GATE 0b1111
#define INTERRUPT_GATE 0b1110
#define MASTER_COMMAND 0x20
#define MASTER_DATA 0x21
#define SLAVE_COMMAND 0xA0
#define SLAVE_DATA 0xA1
#define PIC_EOI 0x20
#define TIMER 0x0 
#define KBOARD 0x1
#define TVECTOR 0x20
#define KVECTOR 0x21

#define IDT_ENTRIES 256

typedef struct context {
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u16 gs __attribute__((aligned(4)));
    u16 fs __attribute__((aligned((4))));
    u16 es __attribute__((aligned(4)));
    u16 ds __attribute__((aligned(4)));
    u8 vector __attribute__((aligned(4)));
    u32 error_code;
    u32 eip;
    u16 cs __attribute__((aligned(4)));
    u32 eflags;
} context;

void init(u16 gate_type);
void io_wait();
void pic_init(u8 aeoi);
void send_eoi(u8 irq);
void clear_mask(u8 irq);
void set_mask(u8 irq);
void setup_handler(u8 irq, u16 vector, void (*handler)(context* ctx));
void setup_handler_no_irq(u16 vector, void (*handler)(context* ctx));
void universal_handler(context* ctx);
#endif
