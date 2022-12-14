/*===========================[ (c) JME SOFT ]===================================
FILE        : [ipaddr.c]
CREATED     : 00/06/02 20:01:59		LAST SAVE    : 00/06/02 23:49:56
WHO         : jerome@mycpu Linux 2.2.14
REMARK      :
================================================================================
- highly inspired from ip/ipaddress.c in iproute2 from alexey
==============================================================================*/

/* system include */
#include <stdio.h>
#include <assert.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/errno.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>

/* local include */
#include "libnetlink.h"

typedef struct {
	int		ifindex;
	uint32_t	*addr;
	int		max_elem;
	int		nb_elem;
} iplist_ctx;

/****************************************************************
 NAME	: print_addr				00/06/02 18:24:09
 AIM	: 
 REMARK	:
****************************************************************/
static int get_addrinfo(struct sockaddr_nl *who, struct nlmsghdr *n, void *arg)
{
	struct ifaddrmsg *ifa 	= NLMSG_DATA(n);
	int		len	= n->nlmsg_len;
	iplist_ctx	*ctx	= (iplist_ctx *)arg;
	struct rtattr 	*rta_tb[IFA_MAX+1];
	/* sanity check */
	len -= NLMSG_LENGTH(sizeof(*ifa));
	if (len < 0) {
		fprintf(stderr, "BUG: wrong nlmsg len %d\n", len);
		return -1;
	}
	/* check the message type */
	if (n->nlmsg_type != RTM_NEWADDR )
		return 0;
	/* check it is ipv4 */
	if( ifa->ifa_family != AF_INET)
		return 0;
	/* check it is the good interface */
	if( ifa->ifa_index != ctx->ifindex )
		return 0;
		
	/* parse the attribute */
	memset(rta_tb, 0, sizeof(rta_tb));
	parse_rtattr(rta_tb, IFA_MAX, IFA_RTA(ifa), len);

	if (!rta_tb[IFA_LOCAL])
		rta_tb[IFA_LOCAL] = rta_tb[IFA_ADDRESS];

	if (rta_tb[IFA_LOCAL]) {
		u_char *src = RTA_DATA(rta_tb[IFA_LOCAL]);
		if( ctx->nb_elem >= ctx->max_elem )
			return 0;
		ctx->addr[ctx->nb_elem++] = 	(src[0]<<24) + (src[1]<<16) +
						(src[2]<<8) + src[3];
	}
	return 0;
}


/****************************************************************
 NAME	: ipaddr_list				00/06/02 20:02:23
 AIM	: 
 REMARK	:
****************************************************************/
int ipaddr_list( int ifindex, uint32_t *array, int max_elem )
{
	struct rtnl_handle	rth;
	iplist_ctx	ctx;
	/* init the struct */
	ctx.ifindex	= ifindex;
	ctx.addr	= array;
	ctx.max_elem	= max_elem;
	ctx.nb_elem	= 0;
	/* open the rtnetlink socket */
	if( rtnl_open( &rth, 0) )
		return -1;
	/* send the request */
	if (rtnl_wilddump_request(&rth, AF_INET, RTM_GETADDR) < 0) {
		perror("Cannot send dump request");
		return -1;
	}
	/* parse the answer */
	if (rtnl_dump_filter(&rth, get_addrinfo, &ctx, NULL, NULL) < 0) {
		fprintf(stderr, "Flush terminated\n");
		exit(1);
	}
	
	/* to close the clocket */
 	rtnl_close( &rth );
	
	return ctx.nb_elem;
}


/****************************************************************
 NAME	: ipaddr_add				00/06/02 23:00:58
 AIM	: add or remove 
 REMARK	:
****************************************************************/

int ipaddr_op( int ifindex, uint32_t addr, uint8_t length, int addF )
{
	struct rtnl_handle	rth;
	struct {
		struct nlmsghdr 	n;
		struct ifaddrmsg 	ifa;
		char   			buf[256];
	} req;
	uint32_t bcast;

	memset(&req, 0, sizeof(req));

	req.n.nlmsg_len		= NLMSG_LENGTH(sizeof(struct ifaddrmsg));
	req.n.nlmsg_flags	= NLM_F_REQUEST;
	req.n.nlmsg_type	= addF ? RTM_NEWADDR : RTM_DELADDR;
	req.ifa.ifa_family	= AF_INET;
	req.ifa.ifa_index	= ifindex;
	req.ifa.ifa_prefixlen	= length;
	
	addr = htonl( addr );
	addattr_l(&req.n, sizeof(req), IFA_LOCAL, &addr, sizeof(addr) );
	
	bcast = addr | htonl((1 << (32 - length)) - 1);
	addattr_l(&req.n, sizeof(req), IFA_BROADCAST, &bcast, sizeof(bcast) );


	if (rtnl_open(&rth, 0) < 0){
 		rtnl_close( &rth );
		return -1;
	}
	if (rtnl_talk(&rth, &req.n, 0, 0, NULL, NULL, NULL) < 0) {
 		rtnl_close( &rth );
		return -1;
	 }
	
	/* to close the clocket */
 	rtnl_close( &rth );

	return(0);
}

