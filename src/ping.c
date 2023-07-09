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
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include "ping.h"

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

  return 0;
}

int ping(char *destination, t_options options)
{
  int error;
  int sockfd;

  struct addrinfo *saddr;
  struct addrinfo *daddr;

  unsigned int sent;
  size_t sent_size;
  size_t payload_size = DEFAULT_PAYLOAD_SIZE;

  (void)options;
  signal(SIGINT, handle_sigint);

  // lookup ipv4 addresses
  if ((error = addr_lookup(SOURCE_ADDR, &saddr)) != 0)
    return error;
  if ((error = addr_lookup(destination, &daddr)) != 0)
    return error;

  if (daddr->ai_family != AF_INET)
  {
    print_error("destination", "Address family for hostname not supported.");
    return 1;
  }

  // new socket for raw icmp packets
  if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
  {
    print_error("socket", strerror(errno));
    return errno;
  }

  // set socket option to include ip header
  if ((error = setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &(int){1}, sizeof(int))) < 0)
  {
    print_error("setsockopt", strerror(errno));
    return errno;
  }

  // set socket option to allow datagram broadcast
  if ((error = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &(int){1}, sizeof(int))) < 0)
  {
    print_error("setsockopt", strerror(errno));
    return errno;
  }

  struct timeval timeout = {.tv_sec = 3, .tv_usec = 0};

  // set socket timeout
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof timeout);

  int packet_size = sizeof(struct iphdr) + sizeof(struct icmphdr) + payload_size;
  char *packet = malloc(packet_size);
  if (!packet)
  {
    print_error("malloc", strerror(errno));
    return errno;
  }
  ft_memset(packet, 0, packet_size);

  // assign structs to write packet headers
  struct iphdr *ip = (struct iphdr *)packet;
  struct icmphdr *icmp = (struct icmphdr *)(packet + sizeof(struct iphdr));

  // set ip header fields
  ip->version = 4;
  ip->ihl = 5;
  ip->tos = 0;
  ip->tot_len = htons(packet_size);
  ip->id = 0;
  ip->frag_off = 0;
  ip->ttl = DEFAULT_TTL;
  ip->protocol = IPPROTO_ICMP;
  ip->saddr = ((struct sockaddr_in *)saddr->ai_addr)->sin_addr.s_addr;
  ip->daddr = ((struct sockaddr_in *)daddr->ai_addr)->sin_addr.s_addr;

  // set icmp header fields
  icmp->type = ICMP_ECHO;
  icmp->code = 0;
  icmp->un.echo.id = getpid();

#define NB_PACKET 3

  for (sent = 0; sent < NB_PACKET; ++sent)
  {
    // set icmp header checksum
    ft_memset(packet + sizeof(struct iphdr) + sizeof(struct icmphdr), rand() % 255, payload_size);
    icmp->un.echo.sequence = sequence_number(sent);
    icmp->checksum = 0;
    icmp->checksum = ft_checksum((unsigned short *)icmp, sizeof(struct icmphdr) + payload_size);

    // set icmp timestamp
    struct timeval *timestamp = (struct timeval *)(packet + sizeof(struct iphdr) + sizeof(struct icmphdr) + payload_size);
    gettimeofday(timestamp, NULL);

    // get start time
    struct timeval start_time;

    gettimeofday(&start_time, NULL);

    // send packet
    if ((sent_size = sendto(sockfd, packet, packet_size, 0, daddr->ai_addr, daddr->ai_addrlen)) < 1)
    {
      print_error("sendto", strerror(errno));
      return errno;
    }

    printf("Sent %zu bytes to %s\n", sent_size, destination);

    // listen for response

    // Clean packet
    ft_memset(packet, 0, packet_size);

    // Read socket with recvmsg
    struct msghdr msg;
    struct sockaddr_in sender;
    struct cmsghdr control_buffer;

    struct iovec iov = {.iov_base = packet, .iov_len = packet_size};
    msg.msg_name = &sender;
    msg.msg_namelen = sizeof sender;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = &control_buffer;
    msg.msg_controllen = sizeof control_buffer;
    msg.msg_flags = 0;
    (void)msg;

    // receive packet
    if ((recv(sockfd, packet, packet_size, 0)) < 0)
    {
      print_error("recvmsg", strerror(errno));
      return errno;
    }

    usleep(1000);
  }

  usleep(1000000);

  free(packet);
  freeaddrinfo(saddr);
  freeaddrinfo(daddr);
  close(sockfd);

  return 0;
}

// int recv_packets(int sockfd, struct sockaddr_in *ipv4, struct icmp_packet *packet, struct timeval *start_time, struct timeval *end_time)
//  {
//    struct sockaddr_in sender;
//    socklen_t sender_len = sizeof sender;
//    struct timeval timeout = {.tv_sec = 1, .tv_usec = 0};
//    fd_set read_set;
//    int bytes_received;

//   FD_ZERO(&read_set);
//   FD_SET(sockfd, &read_set);

//   if (select(sockfd + 1, &read_set, NULL, NULL, &timeout) < 0)
//   {
//     print_error("select", strerror(errno));
//     return errno;
//   }

//   if (FD_ISSET(sockfd, &read_set))
//   {
//     if ((bytes_received = recvfrom(sockfd, packet, sizeof *packet, 0, (struct sockaddr *)&sender, &sender_len)) < 0)
//     {
//       print_error("recvfrom", strerror(errno));
//       return errno;
//     }
//     gettimeofday(end_time, NULL);
//   }
//   else
//   {
//     printf("Request timeout for icmp_seq 0\n");
//     return 1;
//   }

//   if (sender.sin_addr.s_addr != ipv4->sin_addr.s_addr)
//   {
//     printf("Received packet from %s instead of %s\n", inet_ntoa(sender.sin_addr), inet_ntoa(ipv4->sin_addr));
//     return 1;
//   }

//   if (packet->header.type != ICMP_ECHOREPLY)
//   {
//     printf("Received packet with type %d instead of %d\n", packet->header.type, ICMP_ECHOREPLY);
//     return 1;
//   }

//   return 0;
// }

// int send_packets(int sockfd, struct sockaddr_in *ipv4)
// {
//   struct icmp_packet packet;
//   struct timeval start_time;
//   struct timeval end_time;

//   bool sent = true;

//   while (ping_loop)
//   {
//     sent = true;
//     ft_memset(&packet, 0, sizeof packet);

//     packet.header = (struct icmphdr){
//         .type = ICMP_ECHO,
//         .code = 0,
//         .checksum = 0,
//         .un.echo.id = getpid(),
//         .un.echo.sequence = 0};

//     ft_memset(packet.payload, 0, DEFAULT_PAYLOAD_SIZE);

//     if (sendto(sockfd, &packet, sizeof packet, 0, (struct sockaddr *)ipv4, sizeof *ipv4) < 0)
//     {
//       print_error("sendto", strerror(errno));
//       return errno;
//     }
//     gettimeofday(&start_time, NULL);

//     if (recv_packets(sockfd, ipv4, &packet, &start_time, &end_time) != 0)
//       sent = false;

//     if (sent)
//       printf("64 bytes from %s: icmp_seq=0 ttl=64 time=%s ms\n", inet_ntoa(ipv4->sin_addr), sprint_timeval_ms(time_diff(start_time, end_time)));

//     sleep(1);
//   }

//   return 0;
// }
