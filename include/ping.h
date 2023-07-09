#ifndef PING_H
#define PING_H

#include <stdint.h>
#include <sys/time.h>
#include <netinet/ip_icmp.h>

#define HELP_FLAG 0x1
#define VERBOSE_FLAG 0x2

#define DEFAULT_PAYLOAD_SIZE 56
#define DEFAULT_TTL 64

#define SOURCE_ADDR "192.168.0.107"

typedef unsigned int t_options;

typedef struct icmp_packet
{
  struct icmphdr header;
  char payload[DEFAULT_PAYLOAD_SIZE];
} t_icmp_packet;

void *ft_memset(void *b, int c, size_t len);
size_t ft_strlen(const char *s);

const char *sprint_timeval_ms(struct timeval tv);
int ping(char *destination, t_options options);
struct timeval time_diff(struct timeval start, struct timeval end);
unsigned short ft_checksum(unsigned short *ptr, int nbytes);
void print_error(const char *type, const char *msg);
unsigned short sequence_number(unsigned short seq);

#endif // PING_H
