#ifndef KBOARD_H
#define KBOARD_H

#include "types.h"
#include "interrupts.h"
#include "ps2.h"

void keyboard_handler(context* ctx);

struct keymods {
    u8 shift, ctrl, alt, caps;
    u8 rshift, lshift, rctrl, lctrl, ralt, lalt;
};

typedef struct keymods keymods;

#endif
