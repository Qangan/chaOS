#include "vga.h"
#include "memory.h"
#include "types.h"
#include <stdarg.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_BUF ((u16 *)0xB8000)

static u8 cx = 0;
static u8 cy = 0;

static color cbg = black;
static color cfg = white;

void clear_screen() {
  memzero(VGA_BUF, VGA_WIDTH * VGA_HEIGHT * 2);
  cx = 0;
  cy = 0;
}

void put_char(char c, u32 x, u32 y) {
  VGA_BUF[y * VGA_WIDTH + x] = (u16)(c) | (u16)((cfg | cbg << 4) << 8);
}

void scroll_down() {
  memmove(VGA_BUF, VGA_BUF + VGA_WIDTH,
          (VGA_HEIGHT - 1) * VGA_WIDTH * sizeof(u16));
  for (u32 x = 0; x < VGA_WIDTH; x++) {
    put_char(0, VGA_WIDTH - 1, x);
  }
  cy = VGA_WIDTH - 1;
  cx = 0;
}

void set_bg(color bg) { cbg = bg; }

void set_fg(color fg) { cfg = fg; }

void change_bg_color(color new_bg) {
  set_bg(new_bg);
  for (u32 i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
    u16 c = VGA_BUF[i] & 0x00FF;
    VGA_BUF[i] = (u16)(c) | (u16)((cfg | cbg << 4) << 8);
  }
}

static void print_char_internal(char c) {
  if (c == '\n') {
    cx = 0;
    cy++;
  } else if (c == '\r') {
    cx = 0;
  } else {
    put_char(c, cx, cy);
    cx++;
    if (cx >= VGA_WIDTH) {
      cx = 0;
      cy++;
    }
  }

  if (cy >= VGA_HEIGHT) {
    scroll_down();
    cy = VGA_HEIGHT - 1;
  }
}

static void print_string(const char *str) {
  while (*str) {
    print_char_internal(*str++);
  }
}

static void print_number_internal(u32 value, u32 radix) {
  const char *digits = "0123456789ABCDEF";
  char buf[32];
  int i = 0;

  if (value == 0) {
    print_char_internal('0');
    return;
  }

  while (value > 0) {
    buf[i++] = digits[value % radix];
    value /= radix;
  };
  while (--i >= 0) {
    print_char_internal(buf[i]);
  }
}

static void print_signed(s32 value) {
  if (value < 0) {
    print_char_internal('-');
    print_number_internal(-value, 10);
  } else {
    print_number_internal(value, 10);
  }
}

void vprintf(const char *fmt, va_list args) {
  while (*fmt) {
    if (*fmt == '%') {
      fmt++;
      switch (*fmt) {
      case 'u': {
        u32 num = va_arg(args, u32);
        print_number_internal(num, 10);
        break;
      }
      case 'd': {
        s32 num = va_arg(args, s32);
        print_signed(num);
        break;
      }
      case 'x': {
        u32 num = va_arg(args, u32);
        print_string("0x");
        print_number_internal(num, 16);
        break;
      }
      case 'b': {
        u32 num = va_arg(args, u32);
        print_string("0b");
        print_number_internal(num, 2);
        break;
      }
      case 'c': {
        char c = (char)va_arg(args, int);
        print_char_internal(c);
        break;
      }
      case 's': {
        const char *str = va_arg(args, const char *);
        if (str) {
          print_string(str);
        } else {
          print_string("%%nullstring%%");
        }
        break;
      }
      case 'p': {
        void *ptr = va_arg(args, void *);
        if (ptr) {
          print_string("0x");
          print_number_internal((u32)ptr, 16);
        } else {
          print_string("%%NULL_PTR%%");
        }
        break;
      }
      case '%':
        print_char_internal('%');
        break;
      default:
        print_char_internal('%');
        print_char_internal(*fmt);
        break;
      }
    } else {
      print_char_internal(*fmt);
    }
    fmt++;
  }
}

void printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

void printfln(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  printf("\n");
}
