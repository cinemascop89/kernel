#include <descriptor_tables.h>
#include <irq.h>
#include <scrn.h>
#include <stdbool.h>

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void init_isrs() {
  idt_set_gate(0, (unsigned)isr0, 0x08, 0x8e);
  idt_set_gate(1, (unsigned)isr1, 0x08, 0x8e);
  idt_set_gate(2, (unsigned)isr2, 0x08, 0x8e);
  idt_set_gate(3, (unsigned)isr3, 0x08, 0x8e);
  idt_set_gate(4, (unsigned)isr4, 0x08, 0x8e);
  idt_set_gate(5, (unsigned)isr5, 0x08, 0x8e);
  idt_set_gate(6, (unsigned)isr6, 0x08, 0x8e);
  idt_set_gate(7, (unsigned)isr7, 0x08, 0x8e);
  idt_set_gate(8, (unsigned)isr8, 0x08, 0x8e);
  idt_set_gate(9, (unsigned)isr9, 0x08, 0x8e);
  idt_set_gate(10, (unsigned)isr10, 0x08, 0x8e);
  idt_set_gate(11, (unsigned)isr11, 0x08, 0x8e);
  idt_set_gate(12, (unsigned)isr12, 0x08, 0x8e);
  idt_set_gate(13, (unsigned)isr13, 0x08, 0x8e);
  idt_set_gate(14, (unsigned)isr14, 0x08, 0x8e);
  idt_set_gate(15, (unsigned)isr15, 0x08, 0x8e);
  idt_set_gate(16, (unsigned)isr16, 0x08, 0x8e);
  idt_set_gate(17, (unsigned)isr17, 0x08, 0x8e);
  idt_set_gate(18, (unsigned)isr18, 0x08, 0x8e);
  idt_set_gate(19, (unsigned)isr19, 0x08, 0x8e);
  idt_set_gate(20, (unsigned)isr20, 0x08, 0x8e);
  idt_set_gate(21, (unsigned)isr21, 0x08, 0x8e);
  idt_set_gate(22, (unsigned)isr22, 0x08, 0x8e);
  idt_set_gate(23, (unsigned)isr23, 0x08, 0x8e);
  idt_set_gate(24, (unsigned)isr24, 0x08, 0x8e);
  idt_set_gate(25, (unsigned)isr25, 0x08, 0x8e);
  idt_set_gate(26, (unsigned)isr26, 0x08, 0x8e);
  idt_set_gate(27, (unsigned)isr27, 0x08, 0x8e);
  idt_set_gate(28, (unsigned)isr28, 0x08, 0x8e);
  idt_set_gate(29, (unsigned)isr29, 0x08, 0x8e);
  idt_set_gate(30, (unsigned)isr30, 0x08, 0x8e);
  idt_set_gate(31, (unsigned)isr31, 0x08, 0x8e);

  irq_install();
}

char *exception_messages[] = {
  "Division By Zero",
  "Debug",
  "Non Maskable Interrupt",
  "Breakpoint",
  "Into Detected Overflow",
  "Out of Bounds",
  "Invalid Opcode",
  "No Coprocessor",
  "Double Fault",
  "Coprocessor Segment Overrun",
  "Bad TSS",
  "Segment Not Present",
  "Stack Fault",
  "General Protection Fault",
  "Page Fault",
  "Unknown Interrupt",
  "Coprocessor Fault",
  "Alignment Check",
  "Machine Check",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
};

void fault_handler(struct regs *r) {
  if (r->int_no < 32) {
    printf("%d : %s Exception!",
           r->int_no, exception_messages[r->int_no]);

    if (r->int_no == 14) { // Page fault
      bool present = !(r->err_code & 0x1); // Page not present
      bool rw = (r->err_code & 0x2) != 0;    // Is write operation
      bool us = (r->err_code & 0x4) != 0;    // In user mode
      bool reserved = (r->err_code & 0x8) != 0;
      bool id = (r->err_code & 0x10) != 0;   // Caused by instruction fetch
      uint32_t faulting_address;
      asm ("mov %%cr2, %0": "=r"(faulting_address));
      printf(" @ 0x%x", faulting_address);
      if (present) printf(" P");
      if (rw) printf(" RW");
      if (us) printf(" US");
      if (reserved) printf( "R");
      if (id) printf(" ID");
    } else if (r->err_code)
      printf(" Error code: %d", r->err_code);
    putch('\n');

    printf("GS=0x%x FS=0x%x ES=0x%x DS=0x%x\n",
           r->gs, r->fs, r->es, r->ds);
    printf("EDI=0x%x ESI=0x%x EBP=0x%x ESP=0x%x \nEBX=0x%x EDX=0x%x ECX=0x%x EAX=0x%x\n",
           r->edi, r->esi, r->ebp, r->esp, r->ebx, r->edx, r->ecx, r->eax);
    printf("EIP=0x%x CS=0x%x EFLAGS=0b%b USERESP=0x%x SS=0x%x\n\n",
           r->eip, r->cs, r->eflags, r->useresp);

    for(;;);
  }
}
