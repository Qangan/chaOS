#ifndef PANIC_H
#define PANIC_H

#include "types.h"
#include <stdarg.h>
#include "asm_utils.h"

void vkernel_panic(const char* fmt, va_list args);
void kernel_panic(const char* fmt, ...);

#define assertEquals(l, r) \
    do { \
        if (!((l) == (r))) { \
            kernel_panic("Assertion failed: %x != %x at %s:%d\n", \
                (l), (r), __FILE__, __LINE__); \
        } \
    } while (0);

#define assert(cond) \
    do { \
        if (!(cond)) { \
            kernel_panic("Assertion failed: %s at %s:%d\n", \
                #cond, __FILE__, __LINE__); \
        } \
    } while (0);
#endif
