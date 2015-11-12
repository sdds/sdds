#!/bin/sh
ip tuntap add name tap0 mode tap
echo 1 > /proc/sys/net/ipv6/conf/all/forwarding
sysctl net.ipv6.conf.tap0.accept_dad=0
ip link set tap0 up
ip -6 addr add fe80::10/64 dev tap0
