#ifndef __IPHEADER_H__
#define __IPHEADER_H__

#include <netinet/in.h>

/* IP header structure */
class IpHeader
{
    public:
        u_int8_t ihl:4;
        u_int8_t version:4;
        u_int8_t tos;
        u_int16_t tot_len;
        u_int16_t id;
        u_int16_t frag_off;
        
        const int IP_RF = 0x8000;			/* Reserved fragment flag */
        const int IP_DF = 0x4000;			/* Dont fragment flag */
        const int IP_MF = 0x2000;			/* More fragments flag */
        const int IP_OFFMASK = 0x1fff;		/* Mask for fragmenting bits */
        
        u_int8_t ttl;
        u_int8_t protocol;
        u_int16_t check;
        u_int32_t saddr;
        u_int32_t daddr;
 
		IpHeader(const char *p);
};

#endif