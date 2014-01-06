#ifndef __STR_H
#define __STR_H

unsigned int read_uint(char *b);
int strlen(const char *str);
int strcmp(const char *s1, const char *s2);
int strspn (const char *s, const char *accept);
char *strpbrk (const char *s, const char *accept);
char* strtok(char *str, const char *delim);
int strcnt(char* str, const char tok); // Returns the occurrences of tok

#endif
