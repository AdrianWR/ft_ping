#include <stdlib.h>

void *ft_memset(void *b, int c, size_t len)
{
  unsigned char *p = b;
  while (len--)
    *p++ = (unsigned char)c;
  return b;
}

void *ft_memcpy(void *dst, const void *src, size_t n)
{
  unsigned char *s;
  unsigned char *d;

  s = (unsigned char *)src;
  d = (unsigned char *)dst;
  while (n-- && dst != src)
    *d++ = *s++;
  return (dst);
}

size_t ft_strlen(const char *s)
{
  return (*s) ? ft_strlen(++s) + 1 : 0;
}
