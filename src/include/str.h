#ifndef __STR_H
#define __STR_H

#include <stddef.h>
#include <stdint.h>

void *memcpy(void *dest, const void *src, size_t count);
void *memset(void *dest, int val, size_t count);
unsigned short *memsetw(unsigned short *dest, unsigned short val, int count);
uint32_t read_uint(uint8_t *b);
size_t strlen(const char *str);
int strcmp(const char *s1, const char *s2);
int strspn (const char *s, const char *accept);
char *strpbrk (const char *s, const char *accept);
char* strtok(char *str, const char *delim);
int strcnt(char* str, const char tok); // Returns the occurrences of tok

#endif
