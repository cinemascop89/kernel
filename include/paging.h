#ifndef __PAGING_H
#define __PAGING_H

typedef struct page {
  unsigned int present : 1;
  unsigned int rw : 1;
  unsigned int user : 1; // Page in user mode
  unsigned int accessed : 1;
  unsigned int dirty : 1;
  unsigned int unused : 7;
  unsigned int frame : 20;
} page_t;

typedef struct page_table {
  page_t pages[1024];
} page_table_t;

typedef struct page_directory {
  page_table_t *tables[1024]; // Pointers to pagetables
  unsigned int tables_phys[1024]; // Pointers to pagetables physical address
  unsigned int phys_addr;
} page_directory_t;

void init_paging(memory_map_t *mmap, int map_size);
void switch_page_dir(page_directory_t *new); // Load 'new' into CR3
page_t* get_page(unsigned int address, int make, page_directory_t *dir); // returns specified page, if make is set, create page
void page_fault(struct regs *r);
#endif
