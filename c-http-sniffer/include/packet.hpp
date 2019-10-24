#ifndef __PACKET_H__
#define __PACKET_H__

#include <time.h>
//#include <stdio.h>
//#include <pthread.h>
#include <sys/types.h>
#include <stdlib.h>
//#include <pcap/pcap.h>

//#include "util.hpp"

class Packet 
{
	public:
		time_t cap_sec;         /* Capture time sec */
		time_t cap_usec;        /* Capture time usec */
		u_int32_t	raw_len;
		u_int32_t 	saddr;		/* 0x: source IP address */
		u_int32_t 	daddr;		/* 0x: destination IP address */
		u_int16_t 	sport;		/* TCP source port */
		u_int16_t 	dport;		/* TCP destination port */
		u_int8_t 	ip_hl;		/* Bytes: IP header length */
		u_int16_t 	ip_tol;		/* Bytes: IP total length */
		u_int8_t	ip_pro;		/* Protocol */
		u_int16_t	ip_pid;		/* (Unused), needed if packet is fragmented */
		u_int32_t 	tcp_seq;	/* TCP sequence number */
		u_int32_t 	tcp_ack;	/* TCP acknowledge number */
		u_int8_t 	tcp_flags;	/* TCP flags */
		u_int16_t 	tcp_win;	/* TCP window size */
		u_int8_t 	tcp_hl;		/* Bytes: TCP header length */
		u_int16_t	tcp_dl;		/* Bytes: TCP payload length */
		
		static const int HTTP_REQ	= 0x01;
		static const int HTTP_RSP = 0x10;

		u_int8_t	http;		/* is_http or is_request or is_response */
		char		*tcp_odata;	/* Orignal TCP payload */
		char		*tcp_data;	/* Real useful data */
		Packet	*next;		/* Next packet in packet queue */
		
		//char*		time_string;
		
		Packet();
		~Packet();

};

#endif /* __PACKET_H__ */
