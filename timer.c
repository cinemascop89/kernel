#include <system.h>

int timer_ticks = 0;

void timer_handler(struct regs *r) {
  timer_ticks++;
}

void timer_install() {
  irq_install_handler(0, timer_handler);
}

void sleep(float seconds) {
  unsigned long eticks;

  eticks = timer_ticks + seconds * 18;
  while(timer_ticks < eticks) printf(""); // WTF??
}
