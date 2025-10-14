#ifndef MEMORY_UTILS_H
#define MEMORY_UTILS_H

#include "types.h"

void* memmove(void* dst, const void* src, u32 n);
void* memzero(void* dst, u32 n);
void* memset(void* dst, int c, u32 n);
void* immortal_alloc(u32 size, u32 alignment);
void* immortal_calloc(u32 size, u32 alignment);
int memcmp(const void* s1, const void* s2, u32 n);


#endif

