#include <sys/time.h>
#include <stdio.h>

struct timeval time_diff(struct timeval start, struct timeval end)
{
  struct timeval diff;

  diff.tv_sec = end.tv_sec - start.tv_sec;
  diff.tv_usec = end.tv_usec - start.tv_usec;
  if (diff.tv_usec < 0)
  {
    diff.tv_sec--;
    diff.tv_usec += 1000000;
  }
  return diff;
}

char *sprint_timeval_ms(struct timeval tv)
{
  static char buf[30];
  int sec;

  sec = tv.tv_sec;
  sprintf(buf, "%ld,%03ld", sec * 1000 + (tv.tv_usec / 1000), tv.tv_usec % 1000);
  return buf;
}