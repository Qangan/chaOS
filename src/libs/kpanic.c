#include "kpanic.h"
#include "vga.h"

void vkernel_panic(const char *fmt, va_list args) {
  u32 eax, ebx, ecx, edx, esi, edi, ebp, esp;
  u32 eflags;
  u16 cs, ds, es, ss, fs, gs;
  cli();
  clear_screen();
  change_bg_color(white);
  set_fg(black);
  printfln("%s", "Kernel panic :(");
  vprintf(fmt, args);
  printfln("%s", "REGISTERS:");
  // Разбил одну большую вставку на много маленьких потому что компилятору не хватает регистров чтобы всё разом выгрузить
  __asm__ volatile("mov %0, eax\n"
                   "mov %1, ebx\n"
                   "mov %2, ecx\n"
                   "mov %3, edx\n"
                   : "=r"(eax), "=r"(ebx), "=r"(ecx), "=r"(edx));

  __asm__ volatile("mov %0, esi\n"
                   "mov %1, edi\n"
                   "mov %2, ebp\n"
                   "mov %3, esp\n"
                   : "=r"(esi), "=r"(edi), "=r"(ebp), "=r"(esp));

  __asm__ volatile("pushf\n"
                   "pop %0\n"
                   : "=r"(eflags));

  __asm__ volatile("mov ax, cs\n"
                   "mov %0, ax\n"
                   "mov ax, ds\n"
                   "mov %1, ax\n"
                   : "=m"(cs), "=m"(ds));

  __asm__ volatile("mov ax, es\n"
                   "mov %0, ax\n"
                   "mov ax, fs\n"
                   "mov %1, ax\n"
                   : "=m"(es), "=m"(fs));

  __asm__ volatile("mov ax, gs\n"
                   "mov %0, ax\n"
                   "mov ax, ss\n"
                   "mov %1, ax\n"
                   : "=m"(gs), "=m"(ss));
  printf("EAX=%x EBX=%x ECX=%x EDX=%x\n", eax, ebx, ecx, edx);
  printf("ESI=%x EDI=%x EBP=%x ESP=%x\n", esi, edi, ebp, esp);
  printf("EFLAGS=%x\n", eflags);
  printf("CS=%x DS=%x ES=%x FS=%x\n", cs, ds, es, fs);
  printf("GS=%x SS=%x\n", gs, ss);
  loop();
}
void kernel_panic(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vkernel_panic(fmt, args);
  va_end(args);
}
