#ifndef __DESCRIPTOR_TABLES_H
#define __DESCRIPTOR_TABLES_H

#include <stdint.h>

struct regs {
  unsigned int gs, fs, es, ds;
  //  unsigned int dr0, dr1, dr2, dr3, dr4, dr5, dr6, dr7;
  unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
  unsigned int int_no, err_code;
  unsigned int eip, cs, eflags, useresp, ss;
};

extern void init_gdt();
extern void init_idt();
extern void init_isrs();
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

#endif
