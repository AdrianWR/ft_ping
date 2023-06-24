#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include "ping.h"

bool ping_loop = true;

void handle_sigint()
{
  ping_loop = false;
  exit(0);
}

int ipv4_lookup(const char *addr, struct addrinfo **ipv4_addr)
{
  struct addrinfo hints;
  int status;

  ft_memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_RAW;
  hints.ai_protocol = IPPROTO_ICMP;

  if ((status = (getaddrinfo(addr, NULL, &hints, ipv4_addr))) != 0)
  {
    print_error(addr, gai_strerror(status));
    return 2;
  }

  return 0;
}

int ping(char *destination, t_options options)
{
  int error;
  int sockfd;
  struct addrinfo *ipv4_addr;
  struct sockaddr_in *ipv4;

  (void)options;
  signal(SIGINT, handle_sigint);

  // lookup ipv4 address
  if ((error = ipv4_lookup(destination, &ipv4_addr)) != 0)
    return error;

  if (ipv4_addr->ai_family != AF_INET)
  {
    print_error("destination", "Address family for hostname not supported.");
    return 1;
  }

  // printf("ping: %s (%s)\n", destination, inet_ntop(ipv4_addr->ai_family, &ipv4->sin_addr, str, INET_ADDRSTRLEN));

  if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
  {
    print_error("socket", strerror(errno));
    return errno;
  }
  if ((error = setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &(int){1}, sizeof(int))) < 0)
  {
    print_error("setsockopt", strerror(errno));
    return errno;
  }
  // set TTL
  if ((error = setsockopt(sockfd, IPPROTO_IP, IP_TTL, &(int){64}, sizeof(int))) < 0)
  {
    print_error("setsockopt", strerror(errno));
    return errno;
  }

  ipv4 = (struct sockaddr_in *)ipv4_addr->ai_addr;
  return send_packets(sockfd, ipv4);
}

int send_packets(int sockfd, struct sockaddr_in *ipv4)
{
  struct icmp_packet packet;
  struct timeval start_time;
  struct timeval end_time;

  while (ping_loop)
  {
  }

  return 0;
}
