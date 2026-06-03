#ifndef ASM_UTILS_H

#include "types.h"

extern void outb(u16 dx, u8 al);
extern u8 inb(u16 dx);
extern void cli();
extern void sti();
extern void loop();
extern void lidt(void* adr);
extern void collect();
extern u32 get_eflags();
extern void restore_ctx(void* ctx);
extern u32 get_esp();

#endif
