#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>

#include <descriptor_tables.h>
#include <irq.h>
#include <timer.h>
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

void kmain()
{
  init_gdt();
  init_idt();
  init_isrs();

  asm volatile ("sti");

  init_video();

  timer_install();
  keyboard_install();

  welcome_message();

  while(1) {
    putch(getch());
  }

  for(;;);
}
