/*===========================[ (c) JME SOFT ]===================================
FILE        : [ipaddr.h]
CREATED     : 00/06/02 20:12:33		LAST SAVE    : 00/06/02 23:47:42
WHO         : jerome@mycpu Linux 2.2.14
REMARK      :
================================================================================

==============================================================================*/

#ifndef __IPADDR_H__
#define __IPADDR_H__
/* system include */
/* local include */

/*@$#[ipaddr.c] global proto. AutoProtoSigV1.1. date: 00/06/02 23:47:40 */
#include "proto.h"
int ipaddr_list PROTO((int ifindex, uint32_t *array, int max_elem));
int ipaddr_op PROTO((int ifindex, uint32_t addr, uint8_t length, int addF));

/*@$% end of AutoProtoSigV1.1 (Dont remove this line) []*/
 

#endif	/* __IPADDR_H__ */

