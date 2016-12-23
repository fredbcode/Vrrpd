#!/bin/sh
# When vrrpd become Backup

#-------------------------------
PATH=/sbin:/bin:/usr/sbin:/usr/bin
#variables
horodate=$(date +%d/%m/%Y_%R)
month=$(date +%b)
##--------------------------------##

echo "Down int"
ifconfig eth1:0 down

exit 0
