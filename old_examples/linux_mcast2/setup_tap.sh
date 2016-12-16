#!/bin/sh
ip tuntap add name tap1 mode tap user $USER
echo 1 > /proc/sys/net/ipv6/conf/all/forwarding
sysctl net.ipv6.conf.tap1.accept_dad=0
ip link set tap1 up
ip -6 addr add fe80::20/64 dev tap1

