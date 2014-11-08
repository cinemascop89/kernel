#include <irq.h>
#include <io.h>
#include <scrn.h>
#include <stdint.h>

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();


void *irq_routines[16] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

void irq_install_handler(uint32_t irq, void (*handler)(struct regs *r)) {
  irq_routines[irq] = handler;
  printf("irq %d installed at 0x%x\n", irq, handler);
}

void irq_uninstall_handler(uint32_t irq) {
  irq_routines[irq] = 0;
}

void irq_remap() {
  uint8_t m1, m2;

  m1 = inportb(PIC1_DATA);
  m2 = inportb(PIC2_DATA);

  outportb(PIC1_COMMAND, ICW1_INIT + ICW1_ICW4);
  outportb(PIC2_COMMAND, ICW1_INIT + ICW1_ICW4);
  outportb(PIC1_DATA, 0x20);
  outportb(PIC2_DATA, 0x28);
  outportb(PIC1_DATA, 0x04);
  outportb(PIC2_DATA, 0x02);
  outportb(PIC1_DATA, ICW4_8086);
  outportb(PIC2_DATA, ICW4_8086);
  outportb(PIC1_DATA, m1);
  outportb(PIC2_DATA, m2);
}

void irq_install() {
  irq_remap();

  idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8e);
  idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8e);
  idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8e);
  idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8e);
  idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8e);
  idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8e);
  idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8e);
  idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8e);
  idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8e);
  idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8e);
  idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8e);
  idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8e);
  idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8e);
  idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8e);
  idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8e);
  idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8e);
}

void irq_handler(struct regs *r) {
  void (*handler)(struct regs *r);

  handler = irq_routines[r->int_no - 32];
  if (handler) {
    handler(r);
  } else {
    printf("no irq handler!\n");
  }
  if (r->int_no >= 40) {
    outportb(0xa0, 0x20);
  }
  outportb(0x20, 0x20);
}
