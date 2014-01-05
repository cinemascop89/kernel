#ifndef __SYSTEM_H
#define __SYSTEM_H
#include <multiboot.h>

#define PIC1		0x20
#define PIC2		0xA0
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define PIC_EOI		0x20

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */


/* MAIN.C */
extern unsigned char *memcpy(unsigned char *dest, const unsigned char *src, int count);
extern unsigned char *memset(unsigned char *dest, unsigned char val, int count);
extern unsigned short *memsetw(unsigned short *dest, unsigned short val, int count);
unsigned char inportb (unsigned short _port);
unsigned int inportw (unsigned short _port);
void inportsw(unsigned short port, void *addr, unsigned long count);
//void inportsw(unsigned short _port, void* buff, unsigned long count);
void outportb (unsigned short _port, unsigned char _data);
void outportw (unsigned short _port, unsigned int _data);
//void outportsw(unsigned short _port, unsigned int _data);

extern void cls();
extern void putch(unsigned char c);
extern void putint(int val, int b);
extern void puts(unsigned char *str);
extern void settextcolor(unsigned char forecolor, unsigned char backcolor);
extern void init_video();

void init_paging();

extern void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
extern void gdt_install();

extern void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
extern void idt_install();

extern void isrs_install();

struct regs {
  unsigned int gs, fs, es, ds;
  //  unsigned int dr0, dr1, dr2, dr3, dr4, dr5, dr6, dr7;
  unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
  unsigned int int_no, err_code;
  unsigned int eip, cs, eflags, useresp, ss;
};

void irq_install();
void irq_install_handler(int irq, void (*handler)(struct regs *r));
void irq_uninstall_handler(int irq);

void timer_install();
void sleep(float seconds);

void keyboard_install();

extern void do_e820();
void init_mem(memory_map_t *mmap, int map_size);
void* malloc(int size);
void free(void* p, int size);
#endif
