#!/bin/bash
# When vrrpd become Master
# 
#-------------------------------
# THIS IS A SAMPLE FILE WITH ONE IP ALIAS 
# ADAPT TO YOUR OWN CONFIGURATION !

PATH=/sbin:/bin:/usr/sbin:/usr/bin
horodate=$(date +%d/%m/%Y_%R)
##--------------------------------##

echo "1" > /proc/sys/net/ipv4/ip_forward

echo "Montage int"
# DON'T FORGET ! MUST BE REMOVED IN backup.sh
# If VIRTUAL IS ENBALED this address share the same mac adress 
ifconfig eth1:0 192.168.14.2 netmask 255.255.255.0 up

# IF NEEDEED
# REFRESH MAC
# ping -c 2 -I 192.168.14.2 192.168.14.254 (GATEWAY)

echo "ROUTE"
/etc/init.d/route.sh

exit 0
