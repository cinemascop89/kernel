#include <kheap.h>

uint32_t placement_addr;

void init_mem() {
  placement_addr = 0x200000; // 2MB
}

uint32_t kmalloc_int(size_t s, bool aligned, uint32_t *phys) {
  // 8 byte align s
  if (s % 8) {
    s &= 0xfffffff8;
    s += 0x8;
  }

  // page align placement_addr
  if (aligned && (placement_addr & 0xfffff000)) {
    placement_addr &= 0xfffff000;
    placement_addr += 0x1000;
  }
  if (phys) {
    *phys = placement_addr;
  }
  uint32_t tmp = placement_addr;
  placement_addr += s;
  return tmp;
}

uint32_t kmalloc_a(size_t s) {
  return kmalloc_int(s, true, 0);
}

uint32_t kmalloc_p(size_t s, uint32_t *phys) {
  return kmalloc_int(s, false, phys);
}
uint32_t kmalloc_ap(size_t s, uint32_t *phys) {
  return kmalloc_int(s, true, phys);
}

uint32_t kmalloc(size_t s) {
  return kmalloc_int(s, false, 0);
}
