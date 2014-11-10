#include <paging.h>

#include <descriptor_tables.h>
#include <scrn.h>
#include <kheap.h>
#include <str.h>

uint32_t *frames;
uint32_t nframes;

page_directory_t *kernel_directory, *current_directory;

extern uint32_t placement_addr;

#define PANIC(msg) printf("KERNEL PANIC! at %s:%d:\n%s\n", __FILE__, __LINE__, msg); for(;;);

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

static void set_frame(uint32_t frame_addr) {
  uint32_t frame = frame_addr/0x1000;
  uint32_t idx = INDEX_FROM_BIT(frame);
  uint32_t off = OFFSET_FROM_BIT(frame);
  frames[idx] |= (1 << off);
}

static void clear_frame(uint32_t frame_addr) {
  uint32_t frame = frame_addr/0x1000;
  uint32_t idx = INDEX_FROM_BIT(frame);
  uint32_t off = OFFSET_FROM_BIT(frame);
  frames[idx] &= ~(1 << off);
}

static uint32_t test_frame(uint32_t frame_addr) {
  uint32_t frame = frame_addr/0x1000;
  uint32_t idx = INDEX_FROM_BIT(frame);
  uint32_t off = OFFSET_FROM_BIT(frame);
  return frames[idx] & (1 << off);
}

static uint32_t first_frame() {
  uint32_t i, j;
  for (i = 0; i < INDEX_FROM_BIT(nframes); i++) {
    if (frames[i] != 0xffffffff) {
      for (j = 0; j < 32; j++) {
        uint32_t to_test = 1 << j;
        if (!(frames[i] & to_test)) {
          return i*4*8+j;
        }
      }
    }
  }
}

void alloc_frame(page_t *page, uint32_t is_kernel, uint32_t is_writeable) {
  if (page->frame != 0) {
    return;
  } else {
    uint32_t idx = first_frame();
    if (idx == (uint32_t)-1) {
      PANIC("No free frames!");
    }
    set_frame(idx*0x1000);
    page->present = 1;
    page->rw = is_writeable ? 1 : 0;
    page->user = is_kernel ? 0 : 1;
    page->frame = idx;
  }
}

void free_frame(page_t *page) {
  uint32_t frame;
  if (!(frame = page->frame)) {
    return;
  } else {
    clear_frame(frame);
    page->frame = 0x0;
  }
}

void init_paging() {
  uint32_t mem_end_page = 0x1000000; //0x7e00000;

  nframes = mem_end_page / 0x1000;
  frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(nframes));
  printf("frames addr:0x%x, size:0x%x\n", frames, INDEX_FROM_BIT(nframes));
  memset(frames, 0, INDEX_FROM_BIT(nframes));

  kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
  printf("dir addr:0x%x, size:0x%x\n", kernel_directory, sizeof(page_directory_t));
  memset(kernel_directory, 0, sizeof(page_directory_t));
  current_directory = kernel_directory;

  uint32_t i;
  for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000) {
    get_page(i, true, kernel_directory);
  }

  i = 0;
  while (i < placement_addr+0x1000) { // Identity-map until placement_addr and some space for kheap
    alloc_frame(get_page(i, true, kernel_directory), 0, 0);
    i += 0x1000;
  }

  for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000) {
    alloc_frame(get_page(i, false, kernel_directory), 0, 0);
  }

  // TODO: register page fault handler

  switch_page_directory(kernel_directory);

  init_kheap();

}

void switch_page_directory(page_directory_t *dir) {
  current_directory = dir;
  asm volatile("mov %0, %%cr3":: "r"(&dir->tablesPhysical));
  uint32_t cr0;
  asm volatile("mov %%cr0, %0": "=r"(cr0));
  cr0 |= 0x80000000;
  asm volatile("mov %0, %%cr0":: "r"(cr0));
}

page_t *get_page(uint32_t addr, bool make, page_directory_t *dir) {

  addr /= 0x1000;

  uint32_t table_idx = addr / 1024;
  if (dir->tables[table_idx]) {
    return &dir->tables[table_idx]->pages[addr%1024];
  } else if (make) {
    uint32_t tmp;
    dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
    memset(dir->tables[table_idx], 0, 0x1000);
    dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, US
    return &dir->tables[table_idx]->pages[addr%1024];
  } else {
    return 0;
  }
}

void page_fault(struct regs *r) {
  uint32_t faulting_address;

  asm ("mov %%cr2, %0": "=r"(faulting_address));

  bool present = !(r->err_code & 0x1); // Page not present
  bool rw = (r->err_code & 0x2) != 0;    // Is write operation
  bool us = (r->err_code & 0x4) != 0;    // In user mode
  bool reserved = (r->err_code & 0x8) != 0;
  bool id = (r->err_code & 0x10) != 0;   // Caused by instruction fetch

  PANIC("Page fault");
}
