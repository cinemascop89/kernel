#include <io.h>

/* unsigned char inportb(unsigned short _port) {
  unsigned char rv;
  __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
  return rv;
}

unsigned int inportw(unsigned short _port) {
  unsigned int rv;
  __asm__ __volatile__ ("inw %1, %0" : "=a" (rv) : "dN" (_port));
  return rv;
}

/* void inportsw(unsigned short _port, void* buff, unsigned long count) { */
/*   unsigned int *rv; */
/*   __asm__ volatile ( */
/*     "cld; rep; insw " */
/*     : "=D" (buff), "=c"(count) */
/*     : "d"(_port), "0"(buff), "1"(count)); */
/*   return rv; */
/* } */

void inportsw(unsigned short port, void *addr, unsigned long count)
{
  __asm__ __volatile__ (
    "cld ; rep ; insw "
    : "=D" (addr), "=c" (count)
    : "d"(port), "0"(addr), "1" (count)
    : "memory"
  );
}

/* extern void outportb(uint16_t port, uint8_t val) { */
/*     asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) ); */
/*     /\* TODO: Is it wrong to use 'N' for the port? It's not a 8-bit constant. *\/ */
/*     /\* TODO: Should %1 be %w1? *\/ */
/* } */
/* void outportb(unsigned short _port, unsigned char _data) { */
/*   __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data)); */
/* } */

void outportw(unsigned short _port, unsigned int _data) {
  __asm__ __volatile__ ("outw %1, %0" : : "dN" (_port), "a" (_data));
}

/* void outportsw(unsigned short _port, unsigned short _data) { */
/*   __asm__ __volatile__ ("outsw %1, %0" : : "dN" (_port), "=S" (_data), "0"(_data) : "%si"); */
/* } */
