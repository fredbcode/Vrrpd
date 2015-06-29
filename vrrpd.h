/*==========================[ (c) GURU SOFT ]===================================
FILE        : [vrrp.h]
CREATED     : 00/02/02 12:12:06		LAST SAVE    : 00/10/04 21:59:46
WHO         : jerome@mycpu Linux 2.2.14
REMARK      :
================================================================================
- This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version
  2 of the License, or (at your option) any later version.
==============================================================================*/

#ifndef __VRRP_H__
#define __VRRP_H__

/* system include */
#include <stdint.h>

#define VRRPD_VERSION	"Advanced Vrrpd 1.14"
/* Scott added 9-4-02 */
#include <syslog.h>
#define vrrpd_log syslog

/* local include */

typedef struct {	/* rfc2338.5.1 */
	uint8_t		vers_type;	/* 0-3=type, 4-7=version */
	uint8_t		vrid;		/* virtual router id */
	uint8_t		priority;	/* router priority */
	uint8_t		naddr;		/* address counter */
	uint8_t		auth_type;	/* authentification type */
	uint8_t		adver_int;	/* advertissement interval(in sec) */
	uint16_t	chksum;		/* checksum (ip-like one) */
/* here <naddr> ip addresses */
/* here authentification infos */
} vrrp_pkt;

/* protocol constants */
#define INADDR_VRRP_GROUP 0xe0000012	/* multicast addr - rfc2338.5.2.2 */
#define VRRP_IP_TTL	255	/* in and out pkt ttl -- rfc2338.5.2.3 */
#define IPPROTO_VRRP	112	/* IP protocol number -- rfc2338.5.2.4*/
#define VRRP_VERSION	2	/* current version -- rfc2338.5.3.1 */
#define VRRP_PKT_ADVERT	1	/* packet type -- rfc2338.5.3.2 */
#define VRRP_PRIO_OWNER	255	/* priority of the ip owner -- rfc2338.5.3.4 */
#define VRRP_PRIO_DFL	100	/* default priority -- rfc2338.5.3.4 */
#define VRRP_PRIO_STOP	0	/* priority to stop -- rfc2338.5.3.4 */
#define VRRP_AUTH_NONE	0	/* no authentification -- rfc2338.5.3.6 */
#define VRRP_AUTH_PASS	1	/* password authentification -- rfc2338.5.3.6 */
#define VRRP_AUTH_AH	2	/* AH(IPSec) authentification - rfc2338.5.3.6 */
#define VRRP_ADVER_DFL	1	/* advert. interval (in sec) -- rfc2338.5.3.7 */
#define VRRP_PREEMPT_DFL 1	/* rfc2338.6.1.2.Preempt_Mode */

/* implementation specific */
#define VRRP_PIDDIR_DFL	"/var/run"		/* dir to store the pid file */
#define VRRP_PID_FORMAT	"vrrpd_%s.pid"	/* pid file format */
#define ETHTOOL_GLINK            0x0000000a /* Get link status (ethtool_value) */

typedef struct {	/* parameters per interface -- rfc2338.6.1.1 */
	int		auth_type;	/* authentification type. VRRP_AUTH_* */
	uint8_t		auth_data[8];	/* authentification data */

	uint32_t	ipaddr;		/* the address of the interface */
	char		hwaddr[6];	/* WORK: lame hardcoded for ethernet !!!! */
	char		*ifname;	/* the device name for this ipaddr */
} vrrp_if;

typedef struct {
	uint32_t	addr;		/* the ip address */
	uint8_t		length;		/* the ip prefix length */
	int		deletable;	/* TRUE if one of my primary addr */
} vip_addr;

typedef struct {	/* parameters per virtual router -- rfc2338.6.1.2 */
	int	vrid;		/* virtual id. from 1(!) to 255 */
	int	priority;	/* priority value */
	int	naddr;		/* number of ip addresses */
	vip_addr *vaddr;	/* point on the ip address array */
	int	adver_int;	/* delay between advertisements(in sec) */	

#if 0	/* dynamically calculated */
	double	skew_time;	/* skew Master_Down_Interval. (256-Prio)/256 */	
	int	mast_down_int;	/* interval for backup to declare master down*/
#endif
	int	preempt;	/* true if a higher prio preempt a lower one */

	int	state;		/* internal state (init/backup/master) */
	int	wantstate;	/* user explicitly wants a state (back/mast) */

	int	sockfd;		/* the socket descriptor */
	
	int	initF;		/* true if the struct is init */
	
	int	no_vmac;	/* dont handle the virtual MAC --rfc2338.7.3 */

	/* rfc2336.6.2 */
	uint32_t	ms_down_timer;
	uint32_t	adver_timer;

	vrrp_if	vif;


	int	ttin_priority;	/* priority value after SIGTTIN */
	int	ttou_priority;	/* priority value after SIGTTOU */

} vrrp_rt;

/* VRRP state machine -- rfc2338.6.4 */
#define VRRP_STATE_INIT	1	/* rfc2338.6.4.1 */
#define VRRP_STATE_BACK	2	/* rfc2338.6.4.2 */
#define VRRP_STATE_MAST	3	/* rfc2338.6.4.3 */
#define VRRP_STATE_NONE	99	/* internal */

#define VRRP_LOG( str )	printf str
#define VRRP_AUTH_LEN	8

#define VRRP_IS_BAD_VID(id) ((id)<1 || (id)>255)	/* rfc2338.6.1.vrid */
#define VRRP_IS_BAD_PRIORITY(p) ((p)<1 || (p)>255)	/* rfc2338.6.1.prio */
#define VRRP_IS_BAD_ADVERT_INT(d) ((d)<1)



/* use the 'tcp sequence number arithmetic' to handle the wraparound.
** VRRP_TIMER_SUB: <0 if t1 precedes t2, =0 if t1 equals t2, >0 if t1 follows t2
*/
#define VRRP_TIMER_SET( val, delta )	(val) = VRRP_TIMER_CLK() + (delta)
#define VRRP_TIMER_SUB( t1, t2 ) ((int32_t)(((uint32_t)t1)-((uint32_t)t2)))
#define VRRP_TIMER_DELTA( val )		VRRP_TIMER_SUB( val, VRRP_TIMER_CLK() )
#define VRRP_TIMER_EXPIRED( val )	((val) && VRRP_TIMER_DELTA(val)<=0)
#define VRRP_TIMER_CLR( val ) 		(val) = 0
#define VRRP_TIMER_IS_RUNNING( val )	(val)
#define VRRP_TIMER_HZ			1000000
uint32_t VRRP_TIMER_CLK( void )
{
	struct timespec tv;
	uint32_t usecs = 0;
    clock_gettime(CLOCK_MONOTONIC,&tv );
    usecs = (uint32_t)(tv.tv_nsec/1000);
    return tv.tv_sec*VRRP_TIMER_HZ+usecs;
}
              

#define VRRP_TIMER_SKEW( srv ) ((256-(srv)->priority)*VRRP_TIMER_HZ/256) 

#define VRRP_MIN( a , b )	( (a) < (b) ? (a) : (b) )
#define VRRP_MAX( a , b )	( (a) > (b) ? (a) : (b) )

#endif	/* __VRRP_H__ */


