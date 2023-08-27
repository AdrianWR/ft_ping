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
  exit(0);
}

int addr_lookup(const char *addr, struct addrinfo **addrinfo)
{
  struct addrinfo hints;
  int status;

  ft_memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_RAW;
  hints.ai_protocol = IPPROTO_ICMP;

  if ((status = (getaddrinfo(addr, NULL, &hints, addrinfo))) != 0)
  {
    print_error(addr, gai_strerror(status));
    return 2;
  }

  if ((*addrinfo)->ai_family != AF_INET)
  {
    print_error("destination", "Address family for hostname not supported.");
    return 1;
  }

  return 0;
}

int ping(char *destination, t_options options)
{
  int error;
  int sockfd;
  struct addrinfo *addr;
  char *packet;
  const char *ipv4_address;
  struct msghdr msghdr;

  size_t sent_size;
  size_t payload_size = DEFAULT_PAYLOAD_SIZE;

  (void)options;
  signal(SIGINT, handle_sigint);

  unsigned int sent;
  unsigned int received;

  // lookup destination ipv4 address
  if ((error = addr_lookup(destination, &addr)) != 0)
    return error;
  ipv4_address = inet_ntop(AF_INET, &((struct sockaddr_in *)addr->ai_addr)->sin_addr,
                           (char[INET_ADDRSTRLEN]){0}, INET_ADDRSTRLEN);

  // new socket for raw icmp packets
  if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
  {
    print_error("socket", strerror(errno));
    return errno;
  }

  // set socket option to allow datagram broadcast
  if ((error = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &(int){1},
                          sizeof(int))) < 0)
  {
    print_error("setsockopt", strerror(errno));
    return errno;
  }

  // set socket timeout
  // struct timeval timeout = {.tv_sec = 3, .tv_usec = 0};
  // setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout,
  // sizeof timeout);

  size_t packet_size = sizeof(struct icmphdr) + payload_size;
  if (!(packet = malloc(packet_size)))
  {
    print_error("malloc", strerror(errno));
    return errno;
  }

  print_ping_start(destination, ipv4_address, payload_size);

  sent = 0;
  received = 0;
  while (ping_loop && sent < 3)
  {
    // create icmp packet
    icmp_packet(packet, packet_size, sent);

    // send packet
    if ((sent_size = sendto(sockfd, packet, packet_size, 0, addr->ai_addr,
                            addr->ai_addrlen)) < 1)
    {
      print_error("sendto", strerror(errno));
      return errno;
    }
    sent++;

    // receive packet
    ft_memset(packet, 0, packet_size);
    msghdr = message_header(packet, packet_size);

    if (recvmsg(sockfd, &msghdr, 0) < 0)
    {
      print_error("recvmsg", strerror(errno));
      return errno;
    }

    print_ping(packet_size, ipv4_address, &msghdr);
    received++;

    // wait 1 second before sending next packet
    usleep(1000000);
  }

  usleep(10);

  print_ping_statistics(sent, received);

  free(packet);
  freeaddrinfo(addr);
  close(sockfd);

  return 0;
}
