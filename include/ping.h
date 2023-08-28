#ifndef PING_H
#define PING_H

#include <netinet/ip_icmp.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#define HELP_FLAG 0x1
#define VERBOSE_FLAG 0x2

#define DEFAULT_PAYLOAD_SIZE 56
#define DEFAULT_TTL 64
#define DEFAULT_PACKET_TOTAL 3

typedef unsigned int t_options;

void *ft_memset(void *b, int c, size_t len);
void *ft_memcpy(void *dst, const void *src, size_t n);
size_t ft_strlen(const char *s);

float timeval_ms(struct timeval tv);
int ping(char *destination, t_options options);
struct timeval time_diff(struct timeval start, struct timeval end);
unsigned short ft_checksum(unsigned short *ptr, int nbytes);
unsigned short sequence_number(unsigned short seq);
void icmp_packet(char *packet, size_t packet_size, unsigned short seq);
struct msghdr message_header(char *packet, size_t packet_size);
float *ping_statistics(float rtt, int n);

void print_ping(size_t packet_size, const char *ipv4_address, struct msghdr *msgh, float *rtt);
void print_ping_start(const char *destination, const char *ip_address,
                      size_t payload_size, t_options options);
void print_ping_statistics(const char *destination, unsigned int sent, unsigned int received);

#endif // PING_H
