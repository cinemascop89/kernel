#include <stdint.h>
#include <str.h>
#include <descriptor_tables.h>

/* Defines a GDT entry. We say packed, because it prevents the
*  compiler from doing things that it thinks is best: Prevent
*  compiler "optimization" by packing */
struct gdt_entry {
  uint16_t limit_low;
  uint16_t base_low;
  uint8_t base_middle;
  uint8_t access;
  uint8_t granularity;
  uint8_t base_high;
}__attribute__((packed));

/* Special pointer which includes the limit: The max bytes
*  taken up by the GDT, minus 1. Again, this NEEDS to be packed */
struct gdt_ptr {
  uint16_t limit;
  uint32_t base;
}__attribute__((packed));

struct gdt_entry gdt[3];
struct gdt_ptr gp;


struct idt_entry {
  uint16_t base_lo;
  uint16_t sel;
  uint8_t always0;
  uint8_t flags;
  uint16_t base_hi;
}__attribute__((packed));

struct idt_ptr {
  uint16_t limit;
  uint32_t base;
}__attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr idtp;


// properly reload the new segment registers
extern void gdt_flush(uint32_t);
extern void idt_flush(uint32_t);

void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
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

extern  void init_gdt() {
  // setup gdt pointer and limit
  gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
  gp.base = &gdt;

  // NULL descriptor
  gdt_set_gate(0, 0, 0, 0, 0);

  // code segment
  gdt_set_gate(1, 0, 0xffffffff, 0x9a, 0xcf);

  // data segment
  gdt_set_gate(2, 0, 0xffffffff, 0x92, 0xcf);

  gdt_flush((uint32_t)&gp);

}


void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
  idt[num].base_lo = (base & 0xffff);
  idt[num].base_hi = (base >> 16) & 0xffff;

  idt[num].always0 = 0;
  idt[num].flags = flags;
  idt[num].sel = sel;
}

extern void init_idt() {
  idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
  idtp.base = &idt;

  memset(&idt, 0, sizeof(struct idt_entry) * 256);
  idt_flush((uint32_t)&idtp);

}
