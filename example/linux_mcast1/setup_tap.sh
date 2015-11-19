#!/bin/sh
#ip tuntap add dev tap0 mode tap user $USER pi
chmod 0666 /dev/net/tun
modprobe tun
tunctl -t tap0 -u $USER
ip link set tap0 multicast on
echo 1 > /proc/sys/net/ipv6/conf/all/forwarding
sysctl net.ipv6.conf.tap0.accept_dad=0
ip -6 addr add fe80::10/64 dev tap0
ip -6 addr add fe80::20/64 dev tap0
ip link set tap0 up
