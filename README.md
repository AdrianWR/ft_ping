# ft_ping

> Ping stands for Packet InterNet Groper. It is a computer network administration software utility used to test the reachability of a host on an Internet Protocol (IP) network. It measures the round-trip time for messages sent from the originating host to a destination computer that are echoed back to the source. The name comes from active sonar terminology that sends a pulse of sound and listens for the echo to detect objects under water, although it is sometimes interpreted as a backronym to packet Internet groper.

## Description

ft_ping is a recreation of the ping command. It sends ICMP ECHO request packets to network hosts and listens for ICMP ECHO_REPLY responses. It measures the round-trip time and packet loss statistics.

For anyone who had to work in debugging network issues, given them simple or as complex as the internet, this tool is a must have. As almost every machine is capable of sending and receiving ICMP packets, we may use this to test the reachability of a host on an IP network, and how long it takes to get there.

## Compilation

This project requires GNU Make and `clang` to compile. Using C99 won't compile the binary, as we are using some new netowrk functions to provide the DNS resolution (namely `getaddrinfo`). To compile, run `make` or `make all` from the repository root directory.

```
git clone
cd ft_ping
make all
```

Given no issue with the compilation, the binary `ft_ping` will be created at the root of the repository, alongside the object files in the `build/` directory.

## Usage

As we need to open a raw socket to send and receive ICMP packets, `ft_ping` must be run as root. The program takes a single argument, the destination to ping, which can be either an IP address or a hostname. The program will resolve the hostname to an IP address using the `getaddrinfo` function.

```
# ./ft_ping [-?] [-v] destination
```

You may use the `-?` flag to display the help message, and the `-v` flag to enable verbose mode. Verbose mode will display the identifier of the ICMP packet, along with some additional information in case of errors.
