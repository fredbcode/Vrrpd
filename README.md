Vrrpd
=====

Advanced Vrrpd

That version has many improvements like monitoring other vrrpd processes and executing a command when changing back and forth from master to backup. You can also use atropos program for view or change global state. 
Tested on Linux only http://numsys.eu

* VRRPD - when an interface change his state to backup, or master, them can have associated up/down scripts
* VRRPD - Ethtool supervision (link up/down)
* VRRPD - multi-interfaces - The Master communicate his state to all another process. If for some reason one process be backup, link down for example, all the system change for backup state.
* VRRPD - Magic packet - If you can't use virtual mac adress, Vrrpd send gratuitous ARP and it can also send magic packet from virtual to gateway
* VRRPD - Is now Compatible with vlan interfaces - with one vmac by vlan -
* VRRPD - Optional subnet mask for the VIP address
* Atropos client - You can use atropos for change or/and know the master's state (for example in supervision script)

With vmac disable Spanning Tree Protocol (STP) on the switch ports, where VRRP is running !
On VM vmac should be disabled (-n)

The Virtual Router Redundancy Protocol (VRRP) is a computer networking protocol that provides for automatic assignment of available Internet Protocol (IP).

Basic configuration:

Server 1:

vrrpd -i eth0 10.16.1.200 -v 51 -M 2 -U /etc/scripts/MASTER.sh -D /etc/scripts/DOWN.sh

vrrpd -i eth1 10.17.1.200 -v 52 -M 2 -U /etc/scripts/MASTER.sh -D /etc/scripts/DOWN.sh

Server 2:

vrrpd -i eth0 10.16.1.200 -v 51 -M 2 -U /etc/scripts/MASTER.sh -D /etc/scripts/DOWN.sh

vrrpd -i eth1 10.17.1.200 -v 52 -M 2 -U /etc/scripts/MASTER.sh -D /etc/scripts/DOWN.sh

For init script you can take a look at: https://github.com/fredbcode/Vrrpd/tree/master/scripts

* i = eth to listen and VIP network adress
* v = VID 51 
* M = M 2 monitoring two process on each machine (9 max), and eth link up/down supervision 
* U = Optional script when VRRPD become master
* D = Optional Script when VRRPD become backup

About U and D, for example you can configure some IP alias (or vlan) addresses who will share the VMAC (in this case don't forget to shutdown this adresses in backup script ...)

The virtual MAC address is automaticaly generated
