#include <kb.h>
#include <system.h>

int kbd_state = 0;

void keyboard_handler(struct regs *r) {
  unsigned char scancode;

  scancode = inportb(0x60);
  if (scancode & 0x80) {
    //key released
  } else {
    putch(kbdus[scancode]);
  }
}

void mouse_handler(struct regs *r) {
  printf("mouse\n");
}

void keyboard_install() {
  irq_install_handler(1, keyboard_handler);
  irq_install_handler(12, mouse_handler);
}
