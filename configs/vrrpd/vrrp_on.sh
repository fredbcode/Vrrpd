#!/bin/bash
PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
#
# Date:11 Sept 2006 yh ps
# Nom:vrrp_on.sh
# 
# Sample
# 
#---------------------------------
# Please take a look -> vrrpd client:  atropos --help
# THIS IS A SAMPLE FILE WITH TWO PROCESS (TWO NICS)
# ADAPT TO YOUR OWN CONFIGURATION !

horodate=$(date +%d/%m/%Y_%R)

# script - vrrp state change -
up=/etc/vrrpd/Master.sh
down=/etc/vrrpd/Backup.sh

#process vrrp
ps=2

#Vrrp,Vid
int0=eth0
id0=11
vrrip0=192.168.11.1

int1=eth1
id1=14
vrrip1=192.168.14.1

#---------------------------------
# TRAITEMENT

#MOUNT vrrpd 
echo "MOUNT vrrpd "
# BY DEFAULT VIRTUAL MAC IS DISABLED (-n for enable) 
# ACTIVATE AT YOUR OWN RICK
# SPANNING TREE MUST BE DISABLED
# IF MAC REFRESH IS TOO SLOW A PING TO THE GATEWAY CAN BE AN ALTERNATIVE (in master & backup script)
# This setting is technically efficient, but it's incompatible with some hardwares and virtual machines

vrrpd -i $int0 -v $id0 -M $ps $vrrip0 -d 2 -U $up -D $down
vrrpd -i $int1 -v $id1 -M $ps $vrrip1 -d 2 -D $down

exit 0
