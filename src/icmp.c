#include <netinet/ip_icmp.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

#include "ping.h"

void icmp_packet(char *packet, size_t packet_size)
{
  static unsigned short id = 0;
  struct icmphdr *icmp_header;

  ft_memset(packet, 0, packet_size);
  icmp_header = (struct icmphdr *)packet;
  icmp_header->type = ICMP_ECHO;
  icmp_header->code = 0;
  icmp_header->un.echo.id = getpid();
  icmp_header->un.echo.sequence = htons(id++);

  // set packet payload as current time and
  // pad next packet bytes with incrementing pattern
  gettimeofday((struct timeval *)(packet + sizeof(struct icmphdr)), NULL);
  for (size_t i = sizeof(struct icmphdr) + sizeof(struct timeval); i < packet_size; i++)
    packet[i] = i - sizeof(struct icmphdr) - sizeof(struct timeval);

  icmp_header->checksum = 0;
  icmp_header->checksum = ft_checksum((unsigned short *)icmp_header, packet_size);
}

struct msghdr message_header(char *packet, size_t packet_size)
{
  struct msghdr msg;
  struct cmsghdr control_buffer;
  struct iovec iov;

  iov.iov_base = packet;
  iov.iov_len = packet_size;

  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  msg.msg_control = &control_buffer;
  msg.msg_controllen = sizeof control_buffer;
  msg.msg_flags = 0;

  return msg;
}