#include <types.h>
#include <str.h>

static char *olds;

unsigned int read_uint(char *b) {
  return (unsigned char)b[0] + (unsigned char)b[1]*0x100 +
    (unsigned char)b[2]*0x10000 + (unsigned char)b[3]*0x1000000;
}

int strlen(const char *str) {
  int len = 0;
  while (str[len] != '\0') {
    len++;
  }
  return len;
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
