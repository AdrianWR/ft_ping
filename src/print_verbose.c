/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_verbose.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aroque <aroque@student.42sp.org.br>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 13:13:10 by aroque            #+#    #+#             */
/*   Updated: 2023/09/12 13:36:26 by aroque           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
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

static void print_original_package(const char *icmp_packet) {
  char *payload;
  struct iphdr *og_iphdr;
  struct icmphdr *og_icmphdr;
  size_t og_payload_size;

  payload = (char *)icmp_packet + sizeof(struct icmphdr);
  og_iphdr = (struct iphdr *)(payload);
  og_icmphdr = (struct icmphdr *)(payload + sizeof(struct iphdr));
  og_payload_size = ntohs(og_iphdr->tot_len) - sizeof(struct iphdr);

  print_iphdr(og_iphdr);
  print_icmphdr(og_icmphdr, og_payload_size);
}

void print_time_exceeded(t_ping *ping) {
  struct iphdr *ip_header;
  size_t payload_size;
  char ip_address[INET_ADDRSTRLEN];

  ip_header = (struct iphdr *)ping->packet;
  if (!inet_ntop(AF_INET, &(ip_header->saddr), ip_address, INET_ADDRSTRLEN))
    fprintf(stderr, "ping: inet_ntop: %s\n", strerror(errno));

  payload_size = ntohs(ip_header->tot_len) - sizeof(struct iphdr);
  printf("%zu bytes from %s: %s\n", payload_size, ip_address,
         "Time to live exceeded");

  if (ping->options & VERBOSE_FLAG)
    print_original_package(ping->packet + sizeof(struct iphdr));
}
