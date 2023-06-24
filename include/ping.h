#ifndef PING_H
#define PING_H

#include <stdint.h>
#include <sys/time.h>

#define HELP_FLAG 0x1
#define VERBOSE_FLAG 0x2

#define DEFAULT_PAYLOAD_SIZE 64;

typedef unsigned int t_options;

typedef struct icmp_packet
{
  uint8_t type;
  uint8_t code;
  uint16_t checksum;
  uint16_t identifier;
  uint16_t sequence_number;
  struct timeval timestamp;
} t_icmp_packet;

void *ft_memset(void *b, int c, size_t len);
size_t ft_strlen(const char *s);

void print_error(const char *type, const char *msg);

int ping(char *destination, t_options options);

struct timeval time_diff(struct timeval start, struct timeval end);
const char *sprint_timeval_ms(struct timeval tv);

#endif // PING_H
