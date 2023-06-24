#include <stdlib.h>

void *ft_memset(void *b, int c, size_t len)
{
  unsigned char *p = b;
  while (len--)
    *p++ = (unsigned char)c;
  return b;
}

size_t ft_strlen(const char *s)
{
  return (*s) ? ft_strlen(++s) + 1 : 0;
}
