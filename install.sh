#!/bin/sh
PATH=/bin:/usr/bin:/sbin:/usr/sbin
a = 0

clear
 
echo "------------------------------------------------------------------------"
echo "               Vrrpd Atropos "
echo "" 
echo ""
echo ""
echo ""
echo ""
echo "			1) Compilation						" 
echo "			2) Compilation & Installation 				"
echo ""
echo ""
read saisie
case $saisie
	in
         1)
                echo "compilation" 
		make clean
		make
		;;
	 
	2)
		killall 2>/dev/null
		if [ "$?" -ne 1 ]
		then                
			echo "Error: Installation: requires package psmisc (killal)"
			exit 0
		fi
		make clean
		make
		cp vrrpd /usr/sbin/ 2>/dev/null
		cp atropos /usr/sbin/ 2>/dev/null
		cp configs/vrrp /etc/init.d/vrrp 2>/dev/null
		mkdir -p /etc/vrrpd 2>/dev/null
		cp configs/vrrp_on.sh /etc/vrrpd/ 2>/dev/null
		cp -Rf configs/vrrpd/* /etc/vrrpd 2>/dev/null
		ln -s /etc/vrrpd/vrrp_on.sh /etc/vrrp.conf 2>/dev/null
		update-rc.d vrrp defaults 2>/dev/null
		echo "Enabling rclevel for vrrp" 
		
		update-rc.d vrrp defaults 2>/dev/null
		if [ -d /run/systemd/system ] 
		then
		        cp configs/vrrp.service /lib/systemd/system/
           		systemctl enable vrrp 
			systemctl daemon reload
		fi
		
		;;
			 
        *)      # Tous les autres cas
                echo "			Erreur de saisi";; 
esac
