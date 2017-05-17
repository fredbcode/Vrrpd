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

# Parameters
# script - vrrp state change -
up=/etc/vrrpd/Master.sh
down=/etc/vrrpd/Backup.sh

# number of process vrrp MUST be equal the process number 
ps=2

#Vrrp,Vid,eg 2 vrrpd
int0=eth0
id0=11
vrrip0=192.168.11.1

int1=eth1
id1=14
vrrip1=192.168.14.1

echo "MOUNT vrrpd "
# BY DEFAULT VIRTUAL MAC IS DISABLED (virtual mac = -n) 
# ACTIVATE AT YOUR OWN RICK
# SPANNING TREE MUST BE DISABLED
# IF MAC REFRESH IS TOO SLOW A PING FROM VIP TO THE GATEWAY CAN BE ALSO AN ALTERNATIVE (in master & backup script)
# Vmac is technically efficient, but it's incompatible with some hardwares and virtual machines

# Directly related with $ps !

# Add or remove here your process the up script is only needed at first

vrrpd -i $int0 -v $id0 -M $ps $vrrip0 -d 2 -U $up -D $down
vrrpd -i $int1 -v $id1 -M $ps $vrrip1 -d 2 -D $down
