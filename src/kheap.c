#include <kheap.h>

#define ASSERT(b) if(!b) { PANIC("Assert Error"); }

extern uint32_t end;
uint32_t placement_addr = (uint32_t)&end;

#define KHEAP_START 0xc0000000
//#define KHEAP_START 0xc00000
#define KHEAP_INITIAL_SIZE 0x100000
#define KHEAP_MAGIC 0xdeadbaba

typedef struct {
  uint8_t free;
  size_t size;
  uint32_t magic;
} kheap_header_t;

typedef struct {
  uint32_t header;
  uint32_t magic;
} kheap_footer_t;

kheap_t *kheap = NULL;


void init_kheap() {
  printf("init kheap 0x%x\n", placement_addr);
  kheap = (kheap_t*)kmalloc(sizeof(kheap_t));
  kheap->start_addr = KHEAP_START;
  printf("inited kheap 0x%x\n", placement_addr);
  kheap->end_addr = KHEAP_START + KHEAP_INITIAL_SIZE;

  kheap_header_t *first_header = (kheap_header_t*)kheap->start_addr;
  first_header->free = 1;
  first_header->size = KHEAP_INITIAL_SIZE - sizeof(kheap_header_t) - sizeof(kheap_footer_t);
  first_header->magic = KHEAP_MAGIC;

}

uint32_t alloc(size_t s, bool aligned) {
  kheap_header_t *header = (kheap_header_t*)kheap->start_addr;
  while(!(header->free && header->size >= s)) {
    header = (uint32_t)header + header->size + sizeof(kheap_header_t) + sizeof(kheap_footer_t);;
    if (header->magic != KHEAP_MAGIC) {
      printf("magic not found! 0x%x @ 0x%x\n", header->magic, header);
      return 0;
    }
    if ((uint32_t)header > kheap->end_addr)
      return 0;
  }
  kheap_footer_t *footer = (uint32_t)header + sizeof(kheap_header_t) + s;
  footer->magic = KHEAP_MAGIC;
  footer->header = (uint32_t)header;

  kheap_header_t *next = (uint32_t)footer + sizeof(kheap_footer_t);
  next->free = 1;
  next->size = header->size - sizeof(kheap_header_t) - sizeof(kheap_footer_t) - s;
  next->magic = KHEAP_MAGIC;

  header->free = 0;
  header->size = s;
  return (uint32_t)header + sizeof(kheap_header_t);
}

void free(void *ptr) {
  kheap_header_t *header = (uint32_t)ptr - sizeof(kheap_header_t);
  ASSERT(header->magic == KHEAP_MAGIC);
  kheap_footer_t *footer = (uint32_t)ptr + header->size;
  ASSERT(footer->magic == KHEAP_MAGIC);

  if (header->free)
    return;

  if ((uint32_t)header > kheap->start_addr) {
    kheap_footer_t *prev_footer = (uint32_t)header - sizeof(kheap_footer_t);
    ASSERT(prev_footer->magic == KHEAP_MAGIC);
    kheap_header_t *prev_header = (kheap_header_t*)prev_footer->header;
    ASSERT(prev_header->magic == KHEAP_MAGIC);
    if (prev_header->free) {
      prev_header->size += header->size + sizeof(kheap_header_t) + sizeof(kheap_footer_t);
      footer->header = prev_header;

      header = prev_header;
    }
  }

  kheap_header_t *next_header = (uint32_t)footer + sizeof(kheap_footer_t);
  ASSERT(next_header->magic == KHEAP_MAGIC);
  if (next_header->free) {
    kheap_footer_t *next_footer = (uint32_t)next_header + next_header->size + sizeof(kheap_header_t);
    next_footer->header = header;
    header->size += next_header->size + sizeof(kheap_header_t) + sizeof(kheap_footer_t);
  }

  header->free = 1;
}

uint32_t kmalloc_int(size_t s, bool aligned, uint32_t *phys) {
  // 8 byte align s
  if (s % 8) {
    s &= 0xfffffff8;
    s += 0x8;
  }

  if (kheap != NULL)
    return alloc(s, aligned);

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
