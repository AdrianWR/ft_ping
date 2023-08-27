#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>

#include "ping.h"

void print_error(const char *type, const char *msg)
{
  if (type)
    fprintf(stderr, "ping: %s: %s\n", type, msg);
  else
    fprintf(stderr, "ping: %s\n", msg);
}

void print_ping_start(const char *destination, const char *ip_address, size_t payload_size)
{
  printf("PING %s (%s): %zu data bytes\n", destination,
         ip_address ? ip_address : "unknown",
         payload_size);
}

static int get_ttl(struct msghdr *msgh)
{
  struct iphdr *ip_header;

  ip_header = (struct iphdr *)msgh->msg_iov->iov_base;
  return ip_header->ttl;
}

static const char *get_timestamp_diff(struct msghdr *msgh)
{
  struct timeval sent_time;
  struct timeval now;
  const char *timeval;

  // get sent time from packet buffer
  timeval = msgh->msg_iov->iov_base + sizeof(struct iphdr) + sizeof(struct icmphdr);
  ft_memcpy(&sent_time, timeval, sizeof(struct timeval));

  gettimeofday(&now, NULL);
  return sprint_timeval_ms(time_diff(sent_time, now));
}

static unsigned int get_sequence_number(struct msghdr *msgh)
{
  struct icmphdr *icmp_header;

  icmp_header = (struct icmphdr *)(msgh->msg_iov->iov_base + sizeof(struct iphdr));
  return sequence_number(icmp_header->un.echo.sequence);
}

void print_ping(size_t packet_size, const char *ipv4_address, struct msghdr *msgh)
{
  unsigned int sequence_number;
  int ttl;
  const char *time_diff;

  sequence_number = get_sequence_number(msgh);
  ttl = get_ttl(msgh);
  time_diff = get_timestamp_diff(msgh);

  printf("%zu bytes from %s: icmp_seq=%d ttl=%d time=%s ms\n",
         packet_size, ipv4_address, sequence_number, ttl, time_diff);
}

void print_ping_statistics(unsigned int sent, unsigned int received)
{
  float loss;

  if (sent == 0)
    return;
  loss = (sent - received) * 100 / (float)sent;
  printf("\n--- ping statistics ---\n");
  printf("%d packets transmitted, %d received, %.1f%% packet loss\n", sent,
         received, loss);
}
