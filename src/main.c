#include <stdio.h>
#include <stdbool.h>
#include "ping.h"

char *parse_params(size_t argc, char *argv[], t_options *options)
{
  size_t optind;
  for (optind = 1; optind < argc && argv[optind][0] == '-'; optind++)
  {
    size_t opt_len = ft_strlen(argv[optind]);
    for (size_t i = 1; i < opt_len; i++)
    {
      switch (argv[optind][i])
      {
      case 'h':
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
  printf("Usage\n\tping [options] <destination>\n\n");
  printf("Options:\n");
  printf("\t<destination>\tdns name or ip address\n");
  printf("\t-h\t\tprint help and exit\n");
  printf("\t-v\t\tverbose output\n");
  return 2;
}

int main(int argc, char *argv[])
{
  char *destination;
  t_options options;

  destination = parse_params(argc, argv, &options);

  if (options & HELP_FLAG)
    return help();

  if (destination == NULL)
  {
    print_error("usage_error", "Destination address required");
    return 1;
  }

  return ping(destination, options);
}