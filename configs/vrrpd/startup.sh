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

if [ "$1" = "" ] 
then
	exit 1
fi

if [ "$1" = 'stop' ] 
then
	pidof vrrpd > /dev/null
        if [ "$?" = 1 ]
	then
               	echo "Pid vrrpd already stopped"
		exit 0
	fi
	while pidof vrrpd 2>/dev/null
        do
		echo "Pid running"
		killall vrrpd
		sleep 5
	done	
	exit 0
fi
if [ "$1" = 'start' ] 
then
#--------------------------------
	# MOUNT vrrpd 
	echo "MOUNT vrrpd "
	# BY DEFAULT VIRTUAL MAC IS DISABLED (virtual mac = -n) 
	# ACTIVATE AT YOUR OWN RICK
	# SPANNING TREE MUST BE DISABLED
	# IF MAC REFRESH IS TOO SLOW A PING FROM VIP TO THE GATEWAY CAN BE ALSO AN ALTERNATIVE (in master & backup script)
	# Vmac is technically efficient, but it's incompatible with some hardwares and virtual machines

	/etc/vrrpd/vrrp_on.sh

	# check if there is the same number of process than $ps
	ps=`grep ps= /etc/vrrp.conf |cut -d "=" -f2` 
	checkproc=`pgrep vrrpd | wc -w`
	if [ "$checkproc" -ne "$ps" ]
	then
		echo "vrrp.conf: ps value must be equal at the number of process !"
		exit 1
	fi
	sleep 5
fi
