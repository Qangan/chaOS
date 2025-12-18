#ifndef USERSPACE_H
#define USERSPACE_H

#include "interrupts.h"
#include "types.h"

typedef struct {
    context ctx;
    u32 esp;
    u16 ss __attribute__((aligned(4)));
} usercontext;

void process(void* entry, void* stack);

#endif
