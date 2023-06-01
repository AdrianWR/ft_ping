# ft_ping

> Ping stands for Packet InterNet Groper. It is a computer network administration software utility used to test the reachability of a host on an Internet Protocol (IP) network. It measures the round-trip time for messages sent from the originating host to a destination computer that are echoed back to the source. The name comes from active sonar terminology that sends a pulse of sound and listens for the echo to detect objects under water, although it is sometimes interpreted as a backronym to packet Internet groper.

## Description

ft_ping is a recreation of the ping command. It sends ICMP ECHO_REQUEST packets to network hosts and listens for ICMP ECHO_REPLY responses. It measures the round-trip time and packet loss statistics.

For anyone who had to work in debugging network issues, given them simple or as complex as the internet, this tool is a must have. As every machine is capable of sending and receiving ICMP packets, we may use this to test the reachability of a host on an IP network, and how long it takes to get there.

## Compilation

This project requires GNU Make and GCC (or Clang) to compile. To compile, run `make` or `make all` from the root of the repository.

```
git clone
cd ft_ping
make all
```

Given no issue with the compilation, the binary `ft_ping` will be created at the root of the repository, alongside the object files in the `build/` directory.

## Usage

```
./ft_ping [-h] [-v] [-c count] [-i interval] [-t ttl] destination
```
