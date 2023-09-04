#include "ping.h"
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

static void print_iphdr(struct iphdr *ip_header) {
  char src_address[INET_ADDRSTRLEN];
  char dst_address[INET_ADDRSTRLEN];

  if (!inet_ntop(AF_INET, &(ip_header->saddr), src_address, INET_ADDRSTRLEN))
    fprintf(stderr, "ping: inet_ntop: %s\n", strerror(errno));
  if (!inet_ntop(AF_INET, &(ip_header->daddr), dst_address, INET_ADDRSTRLEN))
    fprintf(stderr, "ping: inet_ntop: %s\n", strerror(errno));

  printf("IP Hdr Dump:\n");
  for (size_t i = 0; i < sizeof(struct iphdr); i += 2) {
    printf("%02x%02x ", ((unsigned char *)ip_header)[i],
           ((unsigned char *)ip_header)[i + 1]);
  }
  printf("\n");

  printf("Vr\tHL\tTOS\tLen\tID\tFlg\toff\tTTL\tPro\tcks\tSrc\t\tDst\tData\n");
  printf("%d\t%d\t%02x\t%04x\t%04x\t%d\t%04x\t%02x\t%02x\t%04x\t%s\t%s\n",
         ip_header->version, ip_header->ihl, ip_header->tos,
         ntohs(ip_header->tot_len), ntohs(ip_header->id),
         ntohs(ip_header->frag_off) >> 13, ntohs(ip_header->frag_off) & 0x1fff,
         ip_header->ttl, ip_header->protocol, ntohs(ip_header->check),
         src_address, dst_address);
}

static void print_icmphdr(struct icmphdr *icmp_header, size_t payload_size) {
  printf("ICMP: type %d, code %d, size %zu, id 0x%04x, seq 0x%04x\n",
         icmp_header->type, icmp_header->code, payload_size,
         ntohs(icmp_header->un.echo.id), ntohs(icmp_header->un.echo.sequence));
}

static void print_original_package(struct msghdr *msgh) {
  unsigned char *og_packet;
  struct iphdr *og_iphdr;
  struct icmphdr *og_icmphdr;
  size_t og_payload_size;

  og_packet =
      msgh->msg_iov->iov_base + sizeof(struct iphdr) + sizeof(struct icmphdr);

  og_iphdr = (struct iphdr *)og_packet;
  og_icmphdr = (struct icmphdr *)(og_packet + sizeof(struct iphdr));

  og_payload_size = ntohs(og_iphdr->tot_len) - sizeof(struct iphdr);

  print_iphdr(og_iphdr);
  print_icmphdr(og_icmphdr, og_payload_size);
}

void print_time_exceeded(struct msghdr *msgh) {
  struct iphdr *ip_header;
  char ip_address[INET_ADDRSTRLEN];
  size_t payload_size;

  ip_header = (struct iphdr *)msgh->msg_iov->iov_base;

  if (!inet_ntop(AF_INET, &(ip_header->saddr), ip_address, INET_ADDRSTRLEN))
    fprintf(stderr, "ping: inet_ntop: %s\n", strerror(errno));

  payload_size = sizeof(struct icmphdr) + sizeof(struct iphdr) + 8;
  printf("%zu bytes from %s: %s\n", payload_size, ip_address,
         "Time to live exceeded");

  print_original_package(msgh);
}
