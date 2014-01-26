#include <system.h>

/* Defines a GDT entry. We say packed, because it prevents the
*  compiler from doing things that it thinks is best: Prevent
*  compiler "optimization" by packing */
struct gdt_entry {
  unsigned short limit_low;
  unsigned short base_low;
  unsigned char base_middle;
  unsigned char access;
  unsigned char granularity;
  unsigned char base_high;
}__attribute__((packed));

/* Special pointer which includes the limit: The max bytes
*  taken up by the GDT, minus 1. Again, this NEEDS to be packed */
struct gdt_ptr {
  unsigned short limit;
  unsigned int base;
}__attribute__((packed));

struct gdt_entry gdt[3];
struct gdt_ptr gp;

// properly reload the new segment registers
extern void gdt_flush();

void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran) {
  // setup descriptor base addess
  gdt[num].base_low = (base & 0xffff);
  gdt[num].base_middle = (base >> 16) & 0xff;
  gdt[num].base_high = (base >> 24) & 0xff;

  // setup descriptor limits
  gdt[num].limit_low = (limit & 0xffff);
  gdt[num].granularity = ((limit >> 16) & 0x0f);

  // setup granularity and access flags
  gdt[num].granularity |= (gran & 0xf0);
  gdt[num].access = access;
}

void gdt_install() {
  // setup gdt pointer and limit
  gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
  gp.base = &gdt;

  // NULL descriptor
  gdt_set_gate(0, 0, 0, 0, 0);

  // code segment
  gdt_set_gate(1, 0, 0xffffffff, 0x9a, 0xcf);

  // data segment
  gdt_set_gate(2, 0, 0xffffffff, 0x92, 0xcf);

  gdt_flush();

}
