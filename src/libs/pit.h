#ifndef PIT_H

#include "types.h"
#include "interrupts.h"

#define PIT_CMD 0x43
#define PIT_CH0 0x40

void pit_init(u8 ch, u8 l, u8 h, u8 mode, u8 bcd, u32 hz);
void timer_timer(context* ctx);

#endif
