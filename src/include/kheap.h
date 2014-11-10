#ifndef __KHEAP_H
#define __KHEAP_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define KHEAP_START 0xc0000000
#define KHEAP_INITIAL_SIZE 0x100000

void init_kheap();
uint32_t kmalloc_a(size_t s); // page aligned
uint32_t kmalloc_p(size_t s, uint32_t *phys);
uint32_t kmalloc_ap(size_t s, uint32_t *phys);
uint32_t kmalloc(size_t s);

#endif
