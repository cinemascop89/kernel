#include <types.h>
#include <system.h>
#include <paging.h>

#define KHEAP_START         0xC0000000
#define KHEAP_INITIAL_SIZE  0x100000
#define HEAP_INDEX_SIZE   0x20000
#define HEAP_MAGIC        0x123890AB
#define HEAP_MIN_SIZE     0x70000

unsigned int *frames;
unsigned int nframes;

page_directory_t *kernel_directory, *current_directory;

extern unsigned int placement_addr;

unsigned long page_dir[1024] __attribute__((aligned (4096)));
unsigned long low_pagetable[1024] __attribute__((aligned (4096)));

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

static void set_frame(unsigned int frame_addr) {
  unsigned int frame = frame_addr / 0x1000;
  unsigned int idx = INDEX_FROM_BIT(frame);
  unsigned int off = OFFSET_FROM_BIT(frame);
  frames[idx] |= (0x1 << off);
}

static void clear_frame(unsigned int frame_addr) {
  unsigned int frame = frame_addr / 0x1000;
  unsigned int idx = INDEX_FROM_BIT(frame);
  unsigned int off = OFFSET_FROM_BIT(frame);
  frames[idx] &= ~(0x1 << off);
}

static unsigned int test_frame(unsigned int frame_addr) {
  unsigned int frame = frame_addr / 0x1000;
  unsigned int idx = INDEX_FROM_BIT(frame);
  unsigned int off = OFFSET_FROM_BIT(frame);
  return (frames[idx] & (0x1 << off));
}

// Get the first free frame
unsigned int first_frame() {
  unsigned int i, j;
  for(i = 0; i < INDEX_FROM_BIT(nframes); i++) {
    if (frames[i] != 0xffffffff) {
      for (j = 0; j < 32; j++) {
        unsigned int to_test = 0x1 << j;
        if (!(frames[i] & to_test)) {
          return i*4*8+j;
        }
      }
    }
  }
}

void alloc_frame(page_t *page, int is_kernel, int is_writable) {
  if (page->frame != 0) {
    return; // Frame already allocated
  } else {
    unsigned int idx = first_frame();
    if (idx == (unsigned int)-1) { // No free frames
      PANIC("No more free frames");
    }
    set_frame(idx*0x1000);
    page->present = 1;
    page->rw = is_writable ? 1 : 0;
    page->user = is_kernel ? 0 : 1;
    page->frame = idx;
  }
}

void free_frame(page_t *page) {
  unsigned int frame;
  if (!(frame = page->frame)) {
    return; // Frame wasn't allocated
  } else {
    clear_frame(frame);
    page->frame = 0x0;
  }
}

void init_paging(memory_map_t *mmap, int map_size) {
  unsigned int mem_end_page;
  while (mmap < map_size) {
    /* printf("type: 0x%x, addr: 0x%x, size: %d\n", mmap->type, */
    /*        mmap->base_addr_low + mmap->base_addr_high * 0xffffffff, */
    /*        mmap->length_low + mmap->length_high * 0xffffffff); */
    if (mmap->type == 0x01 && mmap->base_addr_low != 0) {
      mem_end_page = mmap->base_addr_low + mmap->base_addr_high * 0xffffffff + mmap->length_low + mmap->length_high * 0xffffffff;
    }
    mmap = (memory_map_t*) ((unsigned int) mmap + mmap->size + sizeof(unsigned int));
  }

  nframes = mem_end_page / 0x1000;
  frames = (unsigned int*)kmalloc(INDEX_FROM_BIT(nframes));
  memset(frames, 0, INDEX_FROM_BIT(nframes));

  kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
  memset(kernel_directory, 0, sizeof(page_directory_t));
  current_directory = kernel_directory;

  int i;
  for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000) {
    get_page(i, 1, kernel_directory);
  }

  i = 0;
  while (i < placement_addr) {
    alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
    i += 0x1000;
  }
  for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000) {
    alloc_frame( get_page(i, 1, kernel_directory), 0, 0);
  }
  irq_install_handler(14, page_fault);
  switch_page_dir(kernel_directory);
}

void page_fault(struct regs *r) {
  unsigned int fault_address;
  __asm__ __volatile__ (
    "mov %%cr2, %0" : "=r"(fault_address)
  );
  int present = !(r->err_code & 0x1);
  int rw = r->err_code & 0x2;
  int us = r->err_code & 0x4;
  int reserved = r->err_code & 0x8;
  int id = r->err_code & 0x10;

  printf("Page fault! ( ");
  if (present) {printf("present ");}
  if (rw) {printf("read-only ");}
  if (us) {printf("user-mode ");}
  if (reserved) {printf("reserved ");}
  printf(" ) at 0x%x\n", fault_address);
  PANIC("Page fault");
}

void switch_page_dir(page_directory_t *dir) {
  current_directory = dir;
  __asm__ __volatile__ (
    "mov %0, %%cr3" : : "r"(&dir->tables_phys)
  );
  unsigned int cr0;
  __asm__ __volatile__ (
    "mov %%cr0, %0" : "=r"(cr0) :
  );
  cr0 |= 0x80000000;
  __asm__ __volatile__ (
    "mov %0, %%cr0" : : "r"(cr0)
  );
  //  for(;;);
}

page_t* get_page(unsigned int address, int make, page_directory_t *dir) {
  address /= 0x1000; // Get address index
  unsigned int table_idx = address / 1024; // Find the address' page table
  if (dir->tables[table_idx]) { // Page is already assigned
    return &dir->tables[table_idx]->pages[address % 1024];
  } else if (make) {
    unsigned int tmp;
    dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
    memset(dir->tables[table_idx], 0, 0x1000);
    dir->tables_phys[table_idx] = tmp | 0x7; // Present, rw, us
    return &dir->tables[table_idx]->pages[address % 1024];
  } else {
    return NULL;
  }
}
