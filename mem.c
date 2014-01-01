#include <system.h>
#include <multiboot.h>

#define CHUNK_SIZE 16

typedef struct {
  void* next;
  unsigned long size;
} watermark_node;

watermark_node* base_node = 0x00100000;

void init_mem(memory_map_t *mmap, int map_size) {
  unsigned long entry_size, entry_addr;
  while (mmap < map_size) {
    if (mmap->type == 0x01 && mmap->base_addr_low != 0) {
      entry_addr = mmap->base_addr_low + mmap->base_addr_high * 0xffffffff;
      entry_size = mmap->length_low + mmap->length_high * 0xffffffff;
      base_node = (watermark_node*)entry_addr;
      base_node->next = NULL;
      base_node->size = entry_size;
    }
    mmap = (memory_map_t*) ((unsigned int) mmap + mmap->size + sizeof(unsigned int));
  }
}

void* malloc(int size) {
  int allocate_size = size > CHUNK_SIZE ? size : CHUNK_SIZE;
  watermark_node* node = base_node;
  while (node->next != NULL) {
    node = node->next;
  }
  node->size -= allocate_size;
  if (node == base_node) {
    base_node += allocate_size;
    node = base_node;
    node->next = NULL;
  }

  printf("allocated %db node at %x\n",allocate_size, (unsigned int)node);
  return (void*)node;
}
