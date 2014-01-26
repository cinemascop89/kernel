#include <types.h>
#include <system.h>
#include <multiboot.h>

#define CHUNK_SIZE 16

typedef struct {
  void* next;
  unsigned long size;
} watermark_node;

watermark_node* base_node = 0x0080000;
unsigned int placement_addr = 0x0080000;

void init_mem(memory_map_t *mmap, int map_size) {
  unsigned long entry_size, entry_addr;
  while (mmap < map_size) {
    if (mmap->type == 0x01 && mmap->base_addr_low != 0) {
      entry_addr = mmap->base_addr_low + mmap->base_addr_high * 0xffffffff;
      entry_size = mmap->length_low + mmap->length_high * 0xffffffff;
      placement_addr = entry_addr;
      base_node = (watermark_node*)entry_addr;
      base_node->next = NULL;
      base_node->size = entry_size;
    }
    mmap = (memory_map_t*) ((unsigned int) mmap + mmap->size + sizeof(unsigned int));
  }
  //  base_node = (watermark_node*) 0x10; // 0x0 is NULL
  /* base_node->next = NULL; */
  /* base_node->size = 0xc00000; */
  placement_addr = 0x10; //0x400000; // Kernel takes up more then 1mb now
}

unsigned int kmalloc_int(int size, int align, unsigned int *phys) {
  // Check page alignement
  if (align == 1 && (placement_addr & 0xfffff000)) {
    placement_addr &= 0xfffff000;
    placement_addr += 0x1000;
  }
  if (phys) {
    *phys = placement_addr;
  }
  unsigned int tmp = placement_addr;
  placement_addr += size;
  return tmp;
}

unsigned int kmalloc_a(int size) { return kmalloc_int(size, 1, 0); }
unsigned int kmalloc_p(int size, unsigned int *phys) { return kmalloc_int(size, 0, phys); }
unsigned int kmalloc_ap(int size, unsigned int *phys) { return kmalloc_int(size, 1, phys); }
unsigned int kmalloc(int size) { return kmalloc_int(size, 0, 0); }

void* malloc(int size) {
  printf("malloc(%d);\n", size);
  int allocate_size = size > CHUNK_SIZE ? size : CHUNK_SIZE;
  watermark_node* node = base_node;
  // TODO: disabled until we get a decent memory manager
  /* while (node->next != NULL) { */
  /*   printf("next node at %x\n", node->next); */
  /*   node = node->next; */
  /* } */
  //printf("allocating %d bytes\n", size);
  if (node == base_node) {
    base_node += allocate_size/8;
    node = base_node;
    node->next = NULL;
  }

  //  printf("allocated %db node at %x\n",allocate_size, (void*)(node - allocate_size/8));
  return (void*)(node - allocate_size/8);
}

void free(void *p, int size) {

}
