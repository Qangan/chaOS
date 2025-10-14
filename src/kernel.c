#include "libs/kpanic.h"
#include "libs/memory.h"
#include "libs/vga.h"

extern void loop();

void kernel_entry() {
  clear_screen();
  u32 sizes[] = {8, 16, 32, 64, 128, 256, 512, 1024, 2048};
  u32 alignments[] = {2, 3, 5, 7, 13, 17, 19};
  u32 idx = 0;
  u32 cur_pos = 0x7C00 + 100000;

  for (;;) {
    u32 size = sizes[idx % 9];
    u32 align = idx % 111 + 1;
    void *ptr = immortal_alloc(size, align);
    if (cur_pos % align != 0) {
      cur_pos += align - (cur_pos % align);
    }
    assertEquals((u32)ptr, cur_pos);
    cur_pos += size;
    idx += 1;
    printfln("Allocated %u bytes with alignment %u at address %x", size, align,
             ptr);
  }
}
