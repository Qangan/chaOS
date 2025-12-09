#ifndef ASM_UTILS_H

extern void cli();
extern void sti();
extern void loop();
extern void lidt(void* adr);
extern void collect();

#endif
