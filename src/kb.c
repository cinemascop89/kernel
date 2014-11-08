#include <kb.h>
#include <descriptor_tables.h>
#include <stdint.h>
#include <stddef.h>
#include <io.h>
#include <irq.h>

uint32_t kbd_state = 0;
char curr_char = NULL;

void keyboard_handler(struct regs *r) {
  char scancode;

  scancode = inportb(0x60);
  if (scancode & 0x80) {
    //key released
  } else {
    curr_char = kbdus[scancode];
  }
}

void scans(char *s) {
}

char getch() {
  curr_char = NULL;
  while(!curr_char) printf("");
  return curr_char;
}

void keyboard_install() {
  irq_install_handler(1, keyboard_handler);
}
