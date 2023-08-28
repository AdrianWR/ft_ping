#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>

#include "ping.h"

static int get_ttl(struct msghdr *msgh)
{
  struct iphdr *ip_header;

  ip_header = (struct iphdr *)msgh->msg_iov->iov_base;
  return ip_header->ttl;
}

static unsigned int get_sequence_number(struct msghdr *msgh)
{
  struct icmphdr *icmp_header;

  icmp_header = (struct icmphdr *)(msgh->msg_iov->iov_base + sizeof(struct iphdr));
  return sequence_number(icmp_header->un.echo.sequence);
}

static float get_timestamp_diff(struct msghdr *msgh)
{
  struct timeval sent_time;
  struct timeval now;
  const char *timeval;

  // get sent time from packet buffer
  timeval = msgh->msg_iov->iov_base + sizeof(struct iphdr) + sizeof(struct icmphdr);
  ft_memcpy(&sent_time, timeval, sizeof(struct timeval));
  gettimeofday(&now, NULL);

  return timeval_ms(time_diff(sent_time, now));
}

void print_ping_start(const char *destination, const char *ip_address, size_t payload_size, t_options options)
{
  int pid;

  pid = getpid();
  printf("PING %s (%s): %zu data bytes", destination,
         ip_address ? ip_address : "unknown", payload_size);
  if (options & VERBOSE_FLAG)
  {
    printf(", id 0x%x = %d", pid, pid);
  }
  printf("\n");
}

void print_ping(size_t packet_size, const char *ipv4_address, struct msghdr *msgh, float *rtt)
{
  unsigned int sequence_number;
  int ttl;

  sequence_number = get_sequence_number(msgh);
  ttl = get_ttl(msgh);
  *rtt = get_timestamp_diff(msgh);

  printf("%zu bytes from %s: icmp_seq=%d ttl=%d time=%.03f ms\n",
         packet_size, ipv4_address, sequence_number, ttl, *rtt);
}

void print_ping_statistics(const char *destination, unsigned int sent, unsigned int received)
{
  float loss;

  if (sent == 0)
    return;
  loss = (sent - received) * 100 / (float)sent;
  printf("--- %s ping statistics ---\n", destination);
  printf("%d packets transmitted, %d received, %.1f%% packet loss\n", sent,
         received, loss);
}
