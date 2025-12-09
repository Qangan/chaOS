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

struct context;
typedef struct context context;

void init(u16 gate_type);
void io_wait();
void pic_init(u8 aeoi);
void send_eoi(u8 irq);
void clear_mask(u8 irq);
void set_mask(u8 irq);
void setup_handler(u8 irq, u16 vector, void (*handler)(context* ctx));
void universal_handler(struct context* ctx);
