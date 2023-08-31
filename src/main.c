#include "ping.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

t_ping *g_ping;

char *parse_params(size_t argc, char *argv[], unsigned int *options)
{
  size_t optind;
  for (optind = 1; optind < argc && argv[optind][0] == '-'; optind++)
  {
    size_t opt_len = ft_strlen(argv[optind]);
    for (size_t i = 1; i < opt_len; i++)
    {
      switch (argv[optind][i])
      {
      case '?':
        *options |= HELP_FLAG;
        break;
      case 'v':
        *options |= VERBOSE_FLAG;
        break;
      }
    }
  }
  argv += optind;

  return *argv;
}

int help()
{
  printf("Usage: ping [OPTION...] HOST ...\n");
  printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
  printf("Options valid for all request types:\n\n");
  printf(" -v\t\tverbose output\n\n");
  printf("Options valid for --echo requests:\n\n");
  printf(" -?\t\tgive this help list\n");

  return 0;
}

int main(int argc, char *argv[])
{
  char *destination;
  unsigned int options;

  options = 0;
  destination = parse_params(argc, argv, &options);

  if (options & HELP_FLAG)
    return help();

  if (destination == NULL)
  {
    fprintf(stdout, "ping: missing host operand\n");
    fprintf(stdout, "Try 'ping -?' for more information.\n");

    return 64;
  }

  if (!(g_ping = malloc(sizeof(*g_ping))))
  {
    fprintf(stderr, "ping: malloc: %s\n", strerror(errno));
    return 1;
  }
  ft_memset(g_ping, 0, sizeof(*g_ping));

  g_ping->destination = destination;
  g_ping->options = options;

  return ping(g_ping);
}
