#include "kpanic.h"
#include "memory.h"

#define IDT_ENTRIES 256

#define TYPE_ATTR(dpl, gt) (u8)(10000000 | (((dpl) & 0b11) << 5) | ((gt) & 0b1111))

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

#pragma pack(push, 1)
typedef struct {
    u16 limit;
    u32 base;
} idtr_t;

typedef struct idt_entry {
    u16 offset_low;
    u16 selector;
    u8  zero;
    u8  type_attr;
    u16 offset_high;
} idt_entry;

static const u8 error_code_havers[] = {0x8, 0xA, 0xB, 0xC, 0xD, 0xE, 0x11, 0x15};

static u32 has_error_code(u8 vector) {
    for (u32 i = 0;i < 8; i++) {
        if (error_code_havers[i] == vector) {
            return 1;
        }
    }
    return 0;
}
static void* gen_tramps() {
    u8* tramps = (u8*)immortal_alloc(IDT_ENTRIES * 8, 16);
    for (u32 i = 0;i < IDT_ENTRIES; i++) {
        u8* tramp = tramps + i * 8;
        u32 instructions = 0xE9006A00;
        instructions |= ((has_error_code(i) ? 0x90 : 0x50)) | (00 << 8)  | (i << 16) | (00 << 24); // nop | push eax       
        s32 offset = (s32)((u8*)collect_context - (tramp + 8));
        memmove(tramp, &instructions, 4);
        memmove(tramp + 4, &offset, 4);
    }
    return tramps;

}

static void* gen_idt(void* tramps){
    idt_entry* idt = immortal_alloc(IDT_ENTRIES * 8, 16);
    for(u32 i = 1;i < IDT_ENTRIES; i++){
        void* tramp = (u8*)tramps + 8 * i;
        idt[i].offset_low = (u32)tramp & 0xFFFF;
        idt[i].offset_high = ((u32)tramp >> 16) & 0xFFFF;
        idt[i].selector = 0x8;
        idt[i].zero = 0;
        idt[i].type_attr = TYPE_ATTR(0, 0b1110);
    }
    return idt;
}

void init(){
    assertEquals(sizeof(idt_entry), 8);
    void* idt = gen_idt(gen_tramps());
    assertEquals(sizeof(context), 68);
    idtr_t idtr;
    idtr.limit = (IDT_ENTRIES * sizeof(idt_entry)) - 1;
    idtr.base = (u32)idt;
    lidt(&idtr);
}

void universal_handler(context* ctx) {
    kernel_panic(
        "Interrupt: %x at %x:%x\n" 
        "Error Code: %x\n"
        "EAX: %x  EBX: %x  ECX: %x  EDX: %x\n"
        "ESI: %x  EDI: %x  EBP: %x  ESP: %x\n"
        "DS: %x  ES: %x  FS: %x  GS: %x\n"
        "EFLAGS: %b\n",
        ctx->vector, ctx->cs, ctx->eip,
        ctx->error_code,
        ctx->eax, ctx->ebx, ctx->ecx, ctx->edx,
        ctx->esi, ctx->edi, ctx->ebp, ctx->esp,
        ctx->ds, ctx->es, ctx->fs, ctx->gs,
        ctx->eflags
    );}
