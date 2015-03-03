#!/bin/sh
# When vrrpd become Master

#-------------------------------
PATH=/sbin:/bin:/usr/sbin:/usr/bin
#variables
horodate=$(date +%d/%m/%Y_%R)
##--------------------------------##

echo "1" > /proc/sys/net/ipv4/ip_forward

echo "Montage int"
ifconfig eth1:0 192.168.14.2 netmask 255.255.255.0 up

echo "ROUTE"
/etc/init.d/route.sh

exit 0
