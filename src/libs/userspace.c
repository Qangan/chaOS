#include "userspace.h"

void process(void* entry, void* stack){
    usercontext ctx = {
        .ctx = {
            .cs = 0x18 | 3,
            .ds = 0x20 | 3,
            .es = 0x20 | 3,
            .fs = 0x20 | 3,
            .gs = 0x20 | 3,
            .eip = (u32)entry,
            .eflags = (get_eflags() & (~((u32)0b11 << 12))) | (1 << 9) // IOPL 0 and set IF|
    },
        .ss = 0x20 | 3,
        .esp = (u32)stack
    };
    restore_ctx(&ctx);
}
