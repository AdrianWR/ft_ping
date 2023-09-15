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

void handle_interrupt(int signal) {
  int status;

  if (signal == SIGQUIT)
    exit(ENOTRECOVERABLE);

  if (signal == SIGINT) {
    g_ping->ping_loop = false;
    print_ping_statistics(g_ping);
    status = g_ping->received == 0 ? EXIT_FAILURE : EXIT_SUCCESS;

    freeaddrinfo(g_ping->addrinfo);
    free(g_ping->packet);
    close(g_ping->sockfd);
    free(g_ping);

    exit(status);
  }
}

static void send_packet(t_ping *ping) {
  // create icmp packet and send
  icmp_packet(ping->packet, ping->packet_size);
  if ((sendto(ping->sockfd, ping->packet, ping->packet_size, 0,
              ping->addrinfo->ai_addr, ping->addrinfo->ai_addrlen)) < 1) {
    fprintf(stderr, "ping: sendto: %s\n", strerror(errno));
    return;
  }
  ping->sent++;
}

static void receive_message(t_ping *ping) {
  float rtt;
  struct msghdr msghdr;
  struct icmphdr *icmp_header;
  bool recv_loop;

  recv_loop = true;
  ft_memset(ping->packet, 0, ping->packet_size);
  msghdr = message_header(ping->packet, ping->packet_size);

  while (recv_loop) {
    if ((recvmsg(ping->sockfd, &msghdr, 0)) < 0) {
      // timeout, no packet received
      if (errno != EAGAIN || errno != EWOULDBLOCK)
        fprintf(stderr, "ping: recvmsg: %s\n", strerror(errno));
      return;
    }

    icmp_header =
        (struct icmphdr *)(msghdr.msg_iov->iov_base + sizeof(struct iphdr));
    if (icmp_header->type == ICMP_TIME_EXCEEDED) {
      print_time_exceeded(ping);
    } else if (icmp_header->type == ICMP_ECHO) {
      continue;
    } else if (icmp_header->type == ICMP_ECHOREPLY) {
      print_ping(ping, &rtt);
      ping->received++;
    }
    recv_loop = false;

    // wait 1 second before sending next packet
    usleep(DEFAULT_USLEEP);
  }
}

int ping(t_ping *ping) {
  signal(SIGINT, handle_interrupt);
  signal(SIGQUIT, handle_interrupt);

  ping->packet_size = sizeof(struct icmphdr) + DEFAULT_PAYLOAD_SIZE;
  if (!(ping->packet = malloc(ping->packet_size))) {
    fprintf(stderr, "ping: malloc: %s\n", strerror(errno));
    return 1;
  }

  // create socket
  if (new_ping_socket(&(ping->sockfd)) != 0)
    return 1;

  // lookup destination ipv4 address
  if ((addr_lookup(ping)) != 0)
    return 1;

  print_ping_start(ping);
  ping->ping_loop = true;
  while (ping->ping_loop) {
    send_packet(ping);
    receive_message(ping);
  }

  return 0;
}
