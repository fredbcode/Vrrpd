Vrrpd
=====

Advanced Vrrpd

That version has many improvements like monitoring other vrrpd processes and executing a command when changing back and forth from master to backup. 

You can also use atropos program for view or change global state. 
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

* i = eth to listen and VIP network adress - 10.16.1.200 is the VIP shared on eth0 -
* v = VID 51 
* M = M 2 monitoring two process on each machine (9 max), and eth link up/down supervision 
* U = Optional script when VRRPD become master
* D = Optional script when VRRPD become backup

**About U and D :** eg, you can configure some IP alias (or vlan) addresses who will share the VMAC (in this case don't forget to shutdown this adresses in backup script ...)

In MASTER.sh

ifconfig eth0:0 192.168.14.2 netmask 255.255.255.0 up

In DOWN.sh

ifconfig eth0:0 down

Of course, you can put whatever you like.

The virtual MAC address is automaticaly generated

So, In our case there are three adresses on Master

10.17.1.200 and 192.168.14.2 on eth0 -> With same VMAC

10.17.1.20 on eth1 -> VMAC

When one NIC is BACKUP, vrrpd process detect a failure and share with the 'other' network interface his own VRRP state.
This prevent newtork problem because single interface failure can cause asymmetrical routing issues

**vrrpd usage :**
```
vrrpd version Advanced Vrrpd 1.11
Usage: vrrpd -i ifname -v vrid [ -M monitor ] [-s] [-a auth] [-p prio] [-z prio] [-x prio] [-nh] ipaddr
  -h       : display this short inlined help
  -n       : Dont handle the virtual mac address
  -i ifname: the interface name to run on
  -v vrid  : the id of the virtual server [1-255]
  -s       : Switch the preemption mode (Enabled by default)
  -a auth  : auth=(none|pw/hexkey|ah/hexkey) hexkey=0x[0-9a-fA-F]+
  -p prio  : Set the priority of this host in the virtual server (dfl: 100)
  -d delay : Set the advertisement interval (in sec) (dfl: 1)
  -z prio  : Set the priority after SIGTTIN (not decrement as default)
  -x prio  : Set the priority after SIGTTOU (not increment as default)
  ipaddr/length   : Should be at the end - IP address(es) of the virtual server and the length of the subnet mask - 
  -V        : display version

 ---------------------------------------------------------------------------
 Frederic Bourgeois http://numsys.eu
 https://github.com/fredbcode/
 Based on (http://sourceforge.net/projects/vrrpd/)

 Supplementary Options: 
  -U	:	(-U <file>): run <file> to become a master)
  -D	:	(-D <file>): run <file> to become a backup)
  -M	:	(-M x) Monitoring process and Network (Max 9)
  If one vrrpd become backup (or died), all other process go to Backup state
  If one network interface failed (ifconfig Down, link) all other process go to Backup state
  Not supported (in part or whole) on all ethernet drivers
 POWER USERS --------------------------------------------------------------
 Magic packet erase arp table (for those who work without vmac - send fake packet to port 1100 -)
  -I ipvip	:	Choose VIP source to send magic packet - Erase vip from mac table -
  -O ipdst	:	Gateway destination
Example vrrpd -S 10.1.1.1 -O 10.1.1.254
```
**Atropos usage:**
```
Atropos 0.70 frederic Bourgeois http://numsys.eu

atropos --backup 		Be backup (caution: Don't use with priority !)
atropos --reduce 		Reduce priority dynamically priority -10
				If vrrpd run with -z : Set the priority after SIGTTIN (not decrement as default)
atropos --increase 		Increase priority dynamically +10 
				If vrrpd run with -x : Set the priority after SIGTTOU (not increment as default)
atropos --help			This Page
atropos --state			Status 
atropos --version		version 
It requires to be run as root 
```
