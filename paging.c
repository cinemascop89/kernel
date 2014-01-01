#include <system.h>

unsigned long page_dir[1024] __attribute__((aligned (4096)));
unsigned long low_pagetable[1024] __attribute__((aligned (4096)));

void init_paging() {
  void *page_dir_p = 0;
  void *low_pagetable_p = 0;
  int k = 0;

  page_dir_p = (char*)page_dir + 0x40000000;
  low_pagetable_p = (char*)low_pagetable + 0x40000000;
  for (k = 0; k < 1024; k++) {
    low_pagetable[k] = (k*4096) | 0x3;
    page_dir[k] = 0;
  }
  page_dir[0] = (unsigned long)low_pagetable_p | 0x3;
  page_dir[768] = (unsigned long)low_pagetable_p | 0x3;

  __asm__ __volatile__ (
    "mov %0, %%eax\n"
    "mov %%eax, %%cr3\n"
    "mov %%cr0, %%eax\n"
    "orl $0x80000000, %%eax\n"
    "mov %%eax, %%cr0\n" :: "m"(page_dir_p)
  );
}
