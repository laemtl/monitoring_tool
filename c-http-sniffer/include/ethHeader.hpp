#ifndef __ETHHEADER_H__
#define __ETHHEADER_H__

#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>

/* Ethernet header structure */
class EthHeader
{
	public: 
		u_int8_t  ether_dhost[6];		/* Destination addr	*/
		u_int8_t  ether_shost[6];		/* Source addr */
		u_int16_t ether_type;			/* Packet type */

		EthHeader();
		EthHeader(const char *p);
};

#endif