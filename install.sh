#!/bin/sh
PATH=/bin:/usr/bin:/sbin:/usr/sbin

clear
 
echo "------------------------------------------------------------------------"
echo "               Vrrpd Atropos "
echo "" 
echo ""
echo ""
echo ""
echo ""
echo "			1) Compilation (already compiled for debian etch i586) "
echo "			2) Installation"
echo ""
echo ""
read saisie
a=0 
case $saisie
        in
         1)
                echo "			compilation" 
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
		cp vrrpd /usr/sbin/ 2>/dev/null
		cp atropos /usr/sbin/ 2>/dev/null
		cp scripts/vrrp /etc/init.d/vrrp 2>/dev/null
		mkdir -p /etc/VRRP 2>/dev/null
		cp scripts/vrrp_on.sh /etc/VRRP/ 2>/dev/null
		cp -Rf scripts/VRRP/* /etc/VRRP 2>/dev/null
		ln -s /etc/VRRP/vrrp_on.sh /etc/vrrp.conf 2>/dev/null
		update-rc.d vrrp defaults 2>/dev/null
		;;
			 
        *)      # Tous les autres cas
                echo "			Erreur de saisi";; 
esac
