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

ip tuntap add name tap2 mode tap user $USER
echo 1 > /proc/sys/net/ipv6/conf/all/forwarding
sysctl net.ipv6.conf.tap2.accept_dad=0
ip link set tap2 up
ip -6 addr add fd29:144d:4196:94::123/64 dev tap2

ip tuntap add name tap3 mode tap user $USER
echo 1 > /proc/sys/net/ipv6/conf/all/forwarding
sysctl net.ipv6.conf.tap3.accept_dad=0
ip link set tap3 up
ip -6 addr add fe80::219:99ff:fede:9af6/64 dev tap3
