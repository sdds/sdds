#!/bin/sh
ip tuntap add name tap0 mode tap user $USER
echo 1 > /proc/sys/net/ipv6/conf/all/forwarding
sysctl net.ipv6.conf.tap0.accept_dad=0
ip link set tap0 up
ip -6 addr add fe80::c0ef:62ff:fed4:d26a/64 dev tap0

ip tuntap add name tap1 mode tap user $USER
echo 1 > /proc/sys/net/ipv6/conf/all/forwarding
sysctl net.ipv6.conf.tap1.accept_dad=0
ip link set tap1 up
ip -6 addr add fe80::c0ef:62ff:fed4:d36b/64 dev tap1
