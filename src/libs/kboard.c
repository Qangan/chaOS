#include "kboard.h"
#include "./memory.h"
#include "./vga.h"
#include "interrupts.h"

u8 e0 = 0;

keymods mods = {0};

char asciic[128] = {
    0,    27,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',  '=',
    '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[',  ']',
    '\n', 0,    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,    '*',
    0,    ' ',  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,
    0,    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,
    0,    0,    0,   0,   0,   0,   0,   0,   0};

char shifts[128] = {
    0,    27,   '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_',  '+',
    '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{',  '}',
    '\n', 0,    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~',
    0,    '|',  'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,    '*',
    0,    ' ',  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,
    0,    0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,
    0,    0,    0,   0,   0,   0,   0,   0,   0};

void update_modifiers(u16 sc, u8 pressed) {
    if (sc == 0x2A)
        mods.lshift = pressed;
    else if (sc == 0x36)
        mods.rshift = pressed;
    else if (sc == 0x1D)
        mods.lctrl = pressed;
    else if (sc == 0xE01D)
        mods.rctrl = pressed;
    else if (sc == 0x38)
        mods.lalt = pressed;
    else if (sc == 0xE038)
        mods.ralt = pressed;
    else if (sc == 0x3A && pressed)
        mods.caps = !mods.caps;

    mods.shift = mods.lshift || mods.rshift;
    mods.ctrl = mods.lctrl || mods.rctrl;
    mods.alt = mods.lalt || mods.ralt;
}

void handle_scan(u16 sc) {
    u16 pressed = !(sc & 0x80);
    u16 e0f = (sc & 0xE000);
    u16 code = (sc & 0x7F);

    if (e0f)
        code |= 0xE000;
    update_modifiers(code, pressed);

    if (!pressed)
        return;

    if (code == 0x2A || code == 0x36 || code == 0x1D || code == 0xE01D ||
        code == 0x38 || code == 0xE038 || code == 0x3A)
        return;

    char ch = 0;
    if (mods.rctrl) {
        ch = asciic[code];
        if (mods.shift) {
            printf("RCtrl + Shift + %c\n", ch);
            return;
        }
    }
    if (code < 128) {
        if (mods.shift ^ mods.caps && (ch = shifts[code]) && ch >= 'a' &&
            ch <= 'z') {
            ch = asciic[code];
        } else if ((mods.shift || mods.caps) && (ch = shifts[code])) {
            ch = shifts[code];
        } else if ((ch = asciic[code])) {
            ch = asciic[code];
        }
    }
    if (ch)
        printf("%c", ch);
}

void keyboard_handler(context *ctx) {
    u8 sc = inb(KB_DATA);

    if (sc == 0xE0) {
        e0 = 1;
        return;
    }

    u16 code = sc;
    if (e0) {
        code |= 0xE000;
        e0 = 0;
    }
    handle_scan(code);
}
