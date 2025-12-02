#ifndef VGA_H
#define VGA_H

#include "types.h"
#include <stdarg.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

typedef enum {
    black,
    blue,
    green,
    cyan,
    red,
    magenta,
    brown,
    lgrey,
    dgrey,
    lblue,
    lgreen,
    lcyan,
    lred,
    lmagenta,
    lbrown,
    white
} color;

void clear_screen(void);
void put_char(char c, u32 x, u32 y);
void scroll_down(void);

void set_bg(color bg);
void set_fg(color fg);

void change_bg_color(color new_bg);

void vprintf(const char *fmt, va_list args);
void printf(const char *fmt, ...);
void printfln(const char *fmt, ...);

#endif
