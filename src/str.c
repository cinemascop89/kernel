#include <str.h>

static char *olds;

void * memcpy(void *dest, const void *src, size_t count) {
  char* dst8 = (char*)dest;
  char* src8 = (char*)src;

  while (count--) {
    *dst8++ = *src8++;
  }
  return dest;
}

void * memset(void *ptr, int val, size_t count) {
  unsigned char* p=ptr;
  while(count--)
    *p++ = (unsigned char)val;
  return ptr;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, int count) {
  unsigned short i;
  for (i = 0; i < count; i++) {
    dest[i] = val;
  }
  return dest;
}

uint32_t read_uint(uint8_t *b) {
  return (uint8_t)b[0] + (uint8_t)b[1]*0x100 +
    (uint8_t)b[2]*0x10000 + (uint8_t)b[3]*0x1000000;
}

size_t strlen(const char* str)
{
  size_t ret = 0;
  while ( str[ret] != 0 )
    ret++;
  return ret;
}

int strcmp(const char *s1, const char *s2) {
  int i = 0;
  while(s1[i] == s2[i] && s1[i] != '\0' && s2[0] != '\0') i++;
  if (s1[i] == '\0' && s2[i] == '\0') return 0;
  return 1;
}

char *strpbrk (const char *s, const char *accept) {
  while (*s != '\0')
    {
      const char *a = accept;
      while (*a != '\0')
        if (*a++ == *s)
          return (char *) s;
      ++s;
    }
  return NULL;
}

int strcnt(char* str, const char tok) {
  char c;
  int i=0, cnt=0;
  while(c=str[i++]) {
    if (c == tok) cnt++;
  }
  return cnt;
}

int strspn (const char *s, const char *accept) {
  const char *p;
  const char *a;
  int count = 0;

  for (p = s; *p != '\0'; ++p) {
      for (a = accept; *a != '\0'; ++a)
        if (*p == *a)
          break;
      if (*a == '\0')
        return count;
      else
        ++count;
    }
  return count;
}

char *rawmemchr(char* s, char c) {
  while(*s++ != c);
  return s;
}

char* strtok(char *str, const char *delim) {
  char *token;

  if (str == NULL)
    str = olds;

  /* Scan leading delimiters.  */
  str += strspn (str, delim);
  if (*str == '\0')
    {
      olds = str;
      return NULL;
    }

  /* Find the end of the token.  */
  token = str;
  str = strpbrk (token, delim);
  if (str == NULL)
    /* This token finishes the string.  */
    olds = rawmemchr (token, '\0');
  else
    {
      /* Terminate the token and make OLDS point past it.  */
      *str = '\0';
      olds = str + 1;
    }
  return token;
}
