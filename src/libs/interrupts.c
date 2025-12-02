#include "interrupts.h"
#include "kpanic.h"
#include "memory.h"
#include "vga.h"

#define TYPE_ATTR(dpl, gt)                                                     \
    (u8)(10000000 | (((dpl) & 0b11) << 5) | ((gt) & 0b1111))

#define TYPE_ATTR(dpl, gt) (u8)(10000000 | (((dpl) & 0b11) << 5) | ((gt) & 0b1111))
#define JMP 0xE9
#define PUSH 0x6A
#define NOP 0x90
#define PUSHEAX 0x50

void io_wait() { outb(0x80, 0); }

void send_eoi(u8 irq) {
    if (irq >= 8) {
        outb(SLAVE_COMMAND, PIC_EOI);
    }
    outb(MASTER_COMMAND, PIC_EOI);
}

u8 get_master_mask() { return inb(MASTER_DATA); }

u8 get_slave_mask() { return inb(SLAVE_DATA); }

void disable() {
    outb(MASTER_DATA, 0xff);
    outb(SLAVE_DATA, 0xff);
}

void pic_init(u8 aeoi) {
    outb(MASTER_COMMAND, 0b00010001);
    io_wait();
    outb(SLAVE_COMMAND, 0b00010001);
    io_wait();

    outb(MASTER_DATA, 0x20);
    outb(SLAVE_DATA, 0x28);

    outb(MASTER_DATA, 0b00000100);
    io_wait();
    outb(SLAVE_DATA, 2);
    io_wait();

    u8 icw4 = 0b00000001 | (aeoi << 1);

    outb(MASTER_DATA, icw4);
    io_wait();
    outb(SLAVE_DATA, icw4);
    io_wait();

    disable();
}

void set_mask(u8 irq) {
    u16 port;
    u8 value;

    if (irq < 8) {
        port = MASTER_DATA;
    } else {
        port = SLAVE_DATA;
        irq -= 8;
    }
    value = inb(port) | (1 << irq);
    outb(port, value);
}

void clear_mask(u8 irq) {
    u16 port;
    u8 value;

    if (irq < 8) {
        port = MASTER_DATA;
    } else {
        port = SLAVE_DATA;
        irq -= 8;
    }
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

void collect_context();

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

void (*handlers[256])(context *ctx) = {};

#pragma pack(push, 1)
typedef struct {
    u16 limit;
    u32 base;
} idtr_t;

typedef struct idt_entry {
    u16 offset_low;
    u16 selector;
    u8 zero;
    u8 type_attr;
    u16 offset_high;
} idt_entry;

static const u8 error_code_havers[] = {0x8, 0xA, 0xB,  0xC,
                                       0xD, 0xE, 0x11, 0x15};

static u32 has_error_code(u8 vector) {
    for (u32 i = 0; i < 8; i++) {
        if (error_code_havers[i] == vector) {
            return 1;
        }
    }
    return 0;
}

static void* gen_tramps() {
    u8* tramps = (u8*)immortal_alloc(IDT_ENTRIES * sizeof(idt_entry), 16);
    for (u32 vector = 0; vector < IDT_ENTRIES; vector++) {
        u8* tramp = tramps + vector * sizeof(idt_entry);
        u32 instructions = 00 | (PUSH << 8) | (00 << 16) | (JMP << 24);
        instructions |= ((has_error_code(vector) ? NOP : PUSHEAX)) | (00 << 8)  | (vector << 16) | (00 << 24);
        s32 offset = (s32)((u8*)collect_context - (tramp + 8));
        *(u32*)tramp = instructions;
        *(u32*)(tramp + 4) = offset;
        
    }
    return tramps;
}

static void* gen_idt(void* tramps){
    idt_entry* idt = immortal_alloc(IDT_ENTRIES * sizeof(idt_entry), 16);
    for(u32 i = 0; i < IDT_ENTRIES; i++){
        void* tramp = (u8*)tramps + sizeof(idt_entry) * i;
        idt[i].offset_low = (u32)tramp & 0xFFFF;
        idt[i].offset_high = ((u32)tramp >> 16) & 0xFFFF;
        idt[i].selector = 0x8;
        idt[i].zero = 0;
        idt[i].type_attr = TYPE_ATTR(0, gate_type);
    }
    return idt;
}

void init(u16 gate_type) {
    assertEquals(sizeof(idt_entry), 8);
    void *idt = gen_idt(gen_tramps(), gate_type);
    assertEquals(sizeof(context), 68);
    idtr_t idtr;
    idtr.limit = (IDT_ENTRIES * sizeof(idt_entry)) - 1;
    idtr.base = (u32)idt;
    lidt(&idtr);
}

void setup_handler(u8 irq, u16 vector, void (*handler)(context *ctx)) {
    clear_mask(irq);
    handlers[vector] = handler;
}

void universal_handler(context *ctx) {
    if (handlers[ctx->vector]) {
        handlers[ctx->vector](ctx);
        return;
    }
    kernel_panic("Interrupt: %x at %x:%x\n"
                 "Error Code: %x\n"
                 "EAX: %x  EBX: %x  ECX: %x  EDX: %x\n"
                 "ESI: %x  EDI: %x  EBP: %x  ESP: %x\n"
                 "DS: %x  ES: %x  FS: %x  GS: %x\n"
                 "EFLAGS: %b\n",
                 ctx->vector, ctx->cs, ctx->eip, ctx->error_code, ctx->eax,
                 ctx->ebx, ctx->ecx, ctx->edx, ctx->esi, ctx->edi, ctx->ebp,
                 ctx->esp, ctx->ds, ctx->es, ctx->fs, ctx->gs, ctx->eflags);
}
