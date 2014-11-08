#ifndef __IO_H
#define __IO_H

#include <stdint.h>

static inline uint8_t inportb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}
unsigned int inportw (unsigned short _port);
void inportsw(unsigned short port, void *addr, unsigned long count);
void inportsw(unsigned short _port, void* buff, unsigned long count);
static inline void outportb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}
void outportw (unsigned short _port, unsigned int _data);
void outportsw(unsigned short _port, unsigned int _data);

#endif
