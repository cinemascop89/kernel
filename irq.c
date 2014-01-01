#include <system.h>

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

void irq_install_handler(int irq, void (*handler)(struct regs *r)) {
  irq_routines[irq] = handler;
}

void irq_uninstall_handler(int irq) {
  irq_routines[irq] = 0;
}

void irq_remap(void) {
  unsigned char m1, m2;

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

  idt_set_gate(32, (unsigned)irq0, 0x08, 0x8e);
  idt_set_gate(33, (unsigned)irq1, 0x08, 0x8e);
  idt_set_gate(34, (unsigned)irq2, 0x08, 0x8e);
  idt_set_gate(35, (unsigned)irq3, 0x08, 0x8e);
  idt_set_gate(36, (unsigned)irq4, 0x08, 0x8e);
  idt_set_gate(37, (unsigned)irq5, 0x08, 0x8e);
  idt_set_gate(38, (unsigned)irq6, 0x08, 0x8e);
  idt_set_gate(39, (unsigned)irq7, 0x08, 0x8e);
  idt_set_gate(40, (unsigned)irq8, 0x08, 0x8e);
  idt_set_gate(41, (unsigned)irq9, 0x08, 0x8e);
  idt_set_gate(42, (unsigned)irq10, 0x08, 0x8e);
  idt_set_gate(43, (unsigned)irq11, 0x08, 0x8e);
  idt_set_gate(44, (unsigned)irq12, 0x08, 0x8e);
  idt_set_gate(45, (unsigned)irq13, 0x08, 0x8e);
  idt_set_gate(46, (unsigned)irq14, 0x08, 0x8e);
  idt_set_gate(47, (unsigned)irq15, 0x08, 0x8e);
}

void irq_handler(struct regs *r) {
  void (*handler)(struct regs *r);

  handler = irq_routines[r->int_no - 32];
  if (handler) {
    handler(r);
  }
  if (r->int_no >= 40) {
    outportb(0xa0, 0x20);
  }
  outportb(0x20, 0x20);
}
