#include "memory.h"
#include "kpanic.h"
#include "types.h"
#include <stddef.h>

#define IMMORTAL_END 0x80000
#define KERNEL_SIZE 100000
#define IMMORTAL_START (0x7C00 + KERNEL_SIZE)

static u32 current_ptr = IMMORTAL_START;

void *immortal_alloc(u32 size, u32 align) {
    if (current_ptr + size > IMMORTAL_END) {
        kernel_panic("Can't allocate %u bytes from %p. Out of memory.\n", size,
                     current_ptr);
    }

    if (current_ptr % align != 0) {
        current_ptr += align - (current_ptr % align);
    }

    void *ptr = (void *)current_ptr;

    if (((u32)ptr % align) != 0) {
        kernel_panic("Pointer %p is not aligned to %u bytes\n", ptr, align);
    }

    current_ptr += size;

    return ptr;
}

void *immortal_calloc(u32 size, u32 alignment) {
    void *ptr = immortal_alloc(size, alignment);
    if (ptr) {
        memset(ptr, 0, size);
    }
    return ptr;
}

void *memmove(void *dst, const void *src, u32 n) {
    u8 *d = (u8 *)dst;
    const u8 *s = (const u8 *)src;
    if (d < s) {
        for (u32 i = 0; i < n; i++) {
            d[i] = s[i];
        }
    } else {
        for (u32 i = n; i > 0; i--) {
            d[i - 1] = s[i - 1];
        }
    }
    return dst;
}

void *memzero(void *dst, u32 n) {
    u8 *d = (u8 *)dst;
    while (n--) {
        *d++ = 0;
    }
    return dst;
}

void *memset(void *dst, int c, u32 n) {
    u8 *d = (u8 *)dst;
    u8 value = (u8)c;
    while (n--) {
        *d++ = value;
    }
    return dst;
}

int memcmp(const void *s1, const void *s2, u32 n) {
    const u8 *p1 = (const u8 *)s1;
    const u8 *p2 = (const u8 *)s2;

    while (n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}
