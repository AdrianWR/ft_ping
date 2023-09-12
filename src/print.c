#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "ping.h"

static int get_ttl(char *packet) {
  struct iphdr *ip_header;

  ip_header = (struct iphdr *)packet;
  return ip_header->ttl;
}

static unsigned int get_sequence_number(char *packet) {
  struct icmphdr *icmp_header;

  icmp_header = (struct icmphdr *)(packet + sizeof(struct iphdr));
  return htons(icmp_header->un.echo.sequence);
}

static float get_timestamp_diff(char *packet) {
  struct timeval sent_time;
  struct timeval now;
  const char *timeval;

  // get sent time from packet buffer
  timeval = packet + sizeof(struct iphdr) + sizeof(struct icmphdr);
  ft_memcpy(&sent_time, timeval, sizeof(struct timeval));
  gettimeofday(&now, NULL);

  return timeval_ms(time_diff(sent_time, now));
}

void print_ping_start(t_ping *ping) {
  int pid;

  printf("PING %s (%s): %zu data bytes", ping->destination, ping->ip_address,
         (size_t)DEFAULT_PAYLOAD_SIZE);
  if (ping->options & VERBOSE_FLAG) {
    pid = getpid();
    printf(", id 0x%x = %d", pid, pid);
  }
  printf("\n");
}

void print_ping(t_ping *ping, float *rtt) {
  unsigned int sequence_number;
  int ttl;

  sequence_number = get_sequence_number(ping->packet);
  ttl = get_ttl(ping->packet);
  *rtt = get_timestamp_diff(ping->packet);

  printf("%zu bytes from %s: icmp_seq=%d ttl=%d time=%.03f ms\n",
         ping->packet_size, ping->ip_address, sequence_number, ttl, *rtt);
}

void print_ping_statistics(t_ping *ping) {
  float loss;

  printf("--- %s ping statistics ---\n", ping->destination);
  if (ping->sent == 0) {
    printf("0 packets transmitted, 0 received, 0%% packet loss\n");
    return;
  }

  loss = (ping->sent - ping->received) * 100 / (float)ping->sent;
  printf("%d packets transmitted, %d received, %.1f%% packet loss\n",
         ping->sent, ping->received, loss);
}
