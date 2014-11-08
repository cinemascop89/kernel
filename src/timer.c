#include <timer.h>
#include <irq.h>
#include <scrn.h>
#include <descriptor_tables.h>

#include <stdint.h>

uint32_t timer_ticks = 0;

void timer_handler(struct regs *r) {
  timer_ticks++;
}

void timer_install() {
  irq_install_handler(0, timer_handler);
}

void sleep(float seconds) {
  uint32_t eticks;

  eticks = timer_ticks + seconds * 18;
  while(timer_ticks < eticks) printf(""); // WTF??
}
