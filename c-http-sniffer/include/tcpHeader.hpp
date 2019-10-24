#ifndef __TCPHEADER_H__
#define __TCPHEADER_H__

#include <netinet/in.h>

/* TCP header structure */
class TcpHeader 
{
	public:
        u_int16_t th_sport;		/* Source port */
        u_int16_t th_dport;		/* Destination port */
        u_int32_t th_seq;		/* Sequence number */
        u_int32_t th_ack;		/* Acknowledgement number */
        u_int8_t th_x2:4;		/* (Unused) */
        u_int8_t th_off:4;		/* Data offset */
        u_int8_t th_flags;
        u_int16_t th_win;		/* Window */
        u_int16_t th_sum;		/* Checksum */
        u_int16_t th_urp;		/* Urgent pointer */

    	static const int TH_FIN = 0x01;
        static const int TH_SYN = 0x02;
        static const int TH_RST = 0x04;
        static const int TH_PUSH = 0x08;
        static const int TH_ACK = 0x10;
        static const int TH_URG = 0x20;

		TcpHeader(const char *p);
};

#endif