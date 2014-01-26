#include <kb.h>
#include <types.h>
#include <system.h>

int kbd_state = 0;
char curr_char = NULL;

void keyboard_handler(struct regs *r) {
  unsigned char scancode;

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
