#include <stdio.h>

void print_error(const char *type, const char *msg)
{
  if (type)
    fprintf(stderr, "ping: %s: %s\n", type, msg);
  else
    fprintf(stderr, "ping: %s\n", msg);
}