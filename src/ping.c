#include "ping.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

bool ping_loop = true;

void handle_sigint()
{
  ping_loop = false;
}

static int addr_lookup(const char *addr, struct addrinfo **addrinfo)
{
  struct addrinfo hints;
  int status;

  ft_memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_RAW;
  hints.ai_protocol = IPPROTO_ICMP;

  if ((status = (getaddrinfo(addr, NULL, &hints, addrinfo))) != 0)
  {
    if (status == EAI_AGAIN)
      fprintf(stderr, "ping: unknown host\n");
    else
      fprintf(stderr, "ping: %s: %s\n", addr, gai_strerror(status));
    return 1;
  }

  return 0;
}

static int new_ping_socket(int *sockfd)
{
  // new socket for raw icmp packets
  if ((*sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
  {
    fprintf(stderr, "ping: socket: %s\n", strerror(errno));
    return 1;
  }

  // set socket option to allow datagram broadcast
  if ((setsockopt(*sockfd, SOL_SOCKET, SO_BROADCAST, &(int){1}, sizeof(int))) < 0)
  {
    fprintf(stderr, "ping: setsockopt: %s\n", strerror(errno));
    return 1;
  }

  return 0;
}

int ping(char *destination, t_options options)
{
  int sockfd;
  struct addrinfo *addr;
  char *packet;
  size_t packet_size;
  const char *ipv4_address;
  struct msghdr msghdr;
  unsigned int sent;
  unsigned int received;
  size_t payload_size;
  float rtt;
  float stats[4];

  signal(SIGINT, handle_sigint);
  payload_size = DEFAULT_PAYLOAD_SIZE;

  // create socket
  if (new_ping_socket(&sockfd) != 0)
    return 1;

  // lookup destination ipv4 address
  if ((addr_lookup(destination, &addr)) != 0)
    return 1;
  ipv4_address = inet_ntop(AF_INET, &((struct sockaddr_in *)addr->ai_addr)->sin_addr,
                           (char[INET_ADDRSTRLEN]){0}, INET_ADDRSTRLEN);

  packet_size = sizeof(struct icmphdr) + payload_size;
  if (!(packet = malloc(packet_size)))
  {
    fprintf(stderr, "ping: malloc: %s\n", strerror(errno));
    return 1;
  }

  sent = 0;
  received = 0;
  print_ping_start(destination, ipv4_address, payload_size, options);
  while (ping_loop)
  {
    // create icmp packet and send
    icmp_packet(packet, packet_size, received);
    if ((sendto(sockfd, packet,
                packet_size, 0, addr->ai_addr, addr->ai_addrlen)) < 1)
    {
      fprintf(stderr, "ping: sendto: %s\n", strerror(errno));
      return 1;
    }
    sent++;

    // receive packet
    ft_memset(packet, 0, packet_size);
    msghdr = message_header(packet, packet_size);
    if (recvmsg(sockfd, &msghdr, 0) < 0)
    {
      fprintf(stderr, "ping: recvmsg: %s\n", strerror(errno));
      usleep(1000000);
      continue;
    }
    print_ping(packet_size, ipv4_address, &msghdr, &rtt);
    received++;

    ft_memcpy(stats, ping_statistics(rtt, received), sizeof stats);

    // wait 1 second before sending next packet
    usleep(1000000);
  }

  print_ping_statistics(destination, sent, received);
  printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
         stats[0], stats[1], stats[2], stats[3]);

  free(packet);
  freeaddrinfo(addr);
  close(sockfd);

  return 0;
}
