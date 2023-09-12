#ifndef PING_H
#define PING_H

#include <netinet/ip_icmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#define HELP_FLAG 0x1
#define VERBOSE_FLAG 0x2

#define DEFAULT_PAYLOAD_SIZE 56
#define DEFAULT_TTL 64
#define DEFAULT_TIMEOUT 1
#define DEFAULT_USLEEP 1000000

typedef struct s_ping {
  char *destination;
  struct addrinfo *addrinfo;
  int sockfd;
  unsigned int options;
  bool ping_loop;
  unsigned int sent;
  unsigned int received;

  char *packet;
  size_t packet_size;

  char ip_address[INET_ADDRSTRLEN];
} t_ping;

extern t_ping *g_ping;

void *ft_memset(void *b, int c, size_t len);
void *ft_memcpy(void *dst, const void *src, size_t n);
size_t ft_strlen(const char *s);

int ping(t_ping *ping);
int new_ping_socket(int *sockfd);
int addr_lookup(t_ping *ping);
float timeval_ms(struct timeval tv);
struct timeval time_diff(struct timeval start, struct timeval end);
unsigned short ft_checksum(unsigned short *ptr, int nbytes);
void icmp_packet(char *packet, size_t packet_size);
struct msghdr message_header(char *packet, size_t packet_size);
float *ping_statistics(float rtt, int n);

void print_ping(t_ping *ping, float *rtt);
void print_time_exceeded(t_ping *ping);
void print_ping_start(t_ping *ping);
void print_ping_statistics(t_ping *ping);

#endif // PING_H
