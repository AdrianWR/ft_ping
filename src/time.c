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

float timeval_ms(struct timeval tv)
{
  // Return the time in milliseconds from a timeval struct.
  return (float)tv.tv_sec * 1000 + (float)tv.tv_usec / 1000;
}