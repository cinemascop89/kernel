#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>

#include <multiboot.h>
#include <descriptor_tables.h>
#include <irq.h>
#include <timer.h>
#include <kb.h>
#include <kheap.h>
#include <paging.h>
#include <str.h>
#include <scrn.h>
#include <colors.h>

/* Check if the compiler thinks if we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif



#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

void welcome_message() {
  char *welcome = "Welcome!\n";
  size_t i;
  for (i = 0; i < strlen(welcome); i++) {
    settextcolor((i % 14) + 1, BLACK);
    putch(welcome[i]);
  }
  settextcolor(WHITE, BLACK);

}

static inline bool are_interrupts_enabled()
{
    unsigned long flags;
    asm volatile ( "pushf\n\t"
                   "pop %0"
                   : "=g"(flags) );
    return flags & (1 << 9);
}

void show_mmap(multiboot_info_t *mbd) {
  multiboot_memory_map_t *mmap = (multiboot_memory_map_t*)mbd->mmap_addr;
  uint32_t size = 0;
  do {
    //printf("addr: %l, len: %l, type: %d\n", mmap->addr, mmap->len, mmap->type);
      size += mmap->size + sizeof(uint32_t);
      mmap = (multiboot_memory_map_t*)(mbd->mmap_addr + size);
  } while (size < mbd->mmap_length);
}

void kmain(multiboot_info_t *mbd, uint32_t magic) {
  init_gdt();
  init_idt();
  init_isrs();

  asm volatile ("sti");

  init_video();

  timer_install();
  keyboard_install();

  welcome_message();

  printf("Available memory: %d Mb\n", mbd->mem_upper/1024);
  if (magic == MULTIBOOT_BOOTLOADER_MAGIC)
    show_mmap(mbd);

  init_paging();

  printf("paging init!\n");

  uint32_t *a = kmalloc(sizeof(uint32_t));
  *a = 0xaabbccdd;
  uint32_t *b = kmalloc(sizeof(uint32_t));
  *b = 0x44332211;
  printf("a=0x%x @ 0x%x, b=0x%x @ 0x%x\n", *a, a, *b, b);

  free(a);
  printf("a freed\n");
  free(b);
  printf("b freed\n");

  uint32_t *c = kmalloc(sizeof(uint32_t)*2);
  c[0] = 121314;
  c[1] = 161718;
  printf("[%d, %d]\n", c[0], c[1]);

  uint32_t *ptr = (uint32_t*) 0xa0000000;
  printf("fault: 0xd\n", *ptr);

  while(1) {
    putch(getch());
  }

  for(;;);
}
