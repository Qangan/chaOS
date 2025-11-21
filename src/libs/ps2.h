#ifndef PS2_CONTROLLER_H

#include "interrupts.h"

#define KB_DATA 0x60
#define KB_STATUS 0x64
#define KB_CMD 0x64

void ps2_init();

#endif
