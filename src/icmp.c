#include <stdint.h>

unsigned short sequence_number(unsigned short seq)
{
  // sequence number is incremented by 1 for each packet sent
  // if little endian, swap bytes
  seq++;
  if (*(uint16_t *)"\0\xff" > 0x100)
    return ((seq & 0xff) << 8) | ((seq & 0xff00) >> 8);
  return seq;
}