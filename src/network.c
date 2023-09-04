#include "ping.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int addr_lookup(t_ping *ping) {
  struct addrinfo hints;
  int status;

  ft_memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_RAW;
  hints.ai_protocol = IPPROTO_ICMP;

  if ((status = getaddrinfo(ping->destination, NULL, &hints,
                            &(ping->addrinfo))) != 0) {
    if (status == EAI_AGAIN || status == EAI_NONAME)
      fprintf(stderr, "ping: unknown host\n");
    else
      fprintf(stderr, "ping: %s: %s\n", ping->destination,
              gai_strerror(status));
    return 1;
  }

  // get ip address from addrinfo
  if (!inet_ntop(AF_INET,
                 &(((struct sockaddr_in *)(ping->addrinfo->ai_addr))->sin_addr),
                 ping->ip_address, INET_ADDRSTRLEN)) {
    fprintf(stderr, "ping: inet_ntop: %s\n", strerror(errno));
    return 1;
  }

  return 0;
}

int new_ping_socket(int *sockfd) {
  // new socket for raw icmp packets
  if ((*sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
    fprintf(stderr, "ping: socket: %s\n", strerror(errno));
    return 1;
  }

  // set socket option to allow datagram broadcast
  if ((setsockopt(*sockfd, SOL_SOCKET, SO_BROADCAST, &(int){1}, sizeof(int))) <
      0) {
    fprintf(stderr, "ping: setsockopt: %s\n", strerror(errno));
    return 1;
  }

  // set receive timeout to 1 second
  if ((setsockopt(*sockfd, SOL_SOCKET, SO_RCVTIMEO,
                  &(struct timeval){DEFAULT_TIMEOUT, 0},
                  sizeof(struct timeval))) < 0) {
    fprintf(stderr, "ping: setsockopt: %s\n", strerror(errno));
    return 1;
  }

  // set ttl
  // int ttl = 3;
  // if ((setsockopt(*sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)) {
  //  fprintf(stderr, "ping: setsockopt: %s\n", strerror(errno));
  //  return 1;
  //}

  return 0;
}
