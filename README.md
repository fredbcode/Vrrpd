Vrrpd
=====

Advanced Vrrpd

That version has many improvements like monitoring other vrrpd processes and executing a command when changing back and forth from master to backup You can also use atropos program for view or change global state Tested on Linux only http://numsys.eu

* VRRPD - when an interface change his state to backup, or master, them can have associated up/down scripts
* VRRPD - Ethtool supervision (link up/down)
* VRRPD - multi-interfaces - The Master communicate his state to all another process. If for some reason one process be backup, link down for example, all the system change for backup state.
* VRRPD - Magic packet - If you can't use virtual mac adress, Vrrpd send gratuitous ARP and it can also send magic packet from virtual to gateway
* VRRPD - Is now Compatible with vlan interfaces - with one vmac by vlan -
* VRRPD - Optional subnet mask for the VIP address
* Atropos client - You can use atropos for change or/and know the master's state (for example in supervision script)

With vmac disable Spanning Tree Protocol (STP) on the switch ports, where VRRP is running !
On VM vmac should disabled (-n)

