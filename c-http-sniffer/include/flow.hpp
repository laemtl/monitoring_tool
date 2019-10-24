#ifndef __FLOW_H__
#define __FLOW_H__

#include <pthread.h>
#include <sys/time.h>
#include "flowQueue.hpp"
#include "flowHashTable.hpp"
#include "flowSocket.hpp"
#include "tcp.hpp"
#include "http.hpp"
#include "packet.hpp"
#include "order.hpp"

/**
 * Hash management block
 */
class HashMb;

class FlowHashTable;

/* Create a new flow record and initiate its members */
class Flow 
{
	public:
		Flow		*next;
		Flow		*prev;
	/* Packets of flow */
		Packet	*pkt_src_f;		/* front of packets src queue */
		Packet	*pkt_src_e;		/* end of packets src queue */
		Packet	*pkt_dst_f;		/* front of packets dst queue */
		Packet	*pkt_dst_e;		/* end of packets dst queue */
		u_int32_t	pkt_src_n;		/* number of packet in src queue */
		u_int32_t	pkt_dst_n;		/* number of packet in dst queue */
		u_int32_t	pkts_src;       /* total packets count of the flow sent by src */
		u_int32_t	pkts_dst;	    /* total packets count of the flow sent by dst */
		Order		*order;		    /* info to order the packet */
		HashMb	*hmb;		    /* hash management block */
	/* TCP info */
		FlowSocket		socket;		    /* flow socket with 4 tuple */
		u_int8_t	rtt;		    /* round trip time (us) */
		time_t		syn_sec;	    /* syn time in sec */
		time_t		syn_usec;	    /* syn time in usec */
		time_t		ack2_sec;	    /* third handshake time in sec */
		time_t		ack2_usec;	    /* third handshake time in usec */
		time_t		fb_sec;		    /* first byte time of flow payload in sec */
		time_t		fb_usec;	    /* first byte time of flow payload in usec */
		time_t		lb_sec;		    /* last byte time of flow payload in sec */
		time_t		lb_usec;	    /* last byte time of flow payload in usec */
		u_int32_t	payload_src;	/* bytes of payload sent from source to destination */
		u_int32_t 	payload_dst;	/* bytes of payload sent from destination to source */
	/* HTTP info*/
		bool		http;		    /* carrying www ? */
		http_pair_t		*http_f;	/* front of HTTP pair queue */
		http_pair_t		*http_e;	/* end of HTTP pair queue */
		//u_int32_t	http_cnt;	    /* count of HTTP pairs */
	/* Control */
		time_t		last_action_sec;	/* latest modified time to the flow in seconds */
		time_t		last_action_usec;	/* latest modified time to the flow in microseconds */
		u_int8_t	close;
		bool		init;			/* the flow has received its first packet */
		bool 		processed;

		static const int CLIENT_CLOSE = 0x01;
		static const int SERVER_CLOSE = 0x02;
		static const int FORCED_CLOSE = 0x04;

		int raw_req;
		int raw_rsp;

		FlowQueue* fq;
		FlowHashTable* fht; 
		
		Flow(FlowHashTable* fht, FlowQueue* fq);
		~Flow();

		void reset();
		int hook_packet(Packet *packet, bool src);
		int cal_packet(Packet *packet, bool src);

		int add_http(http_pair_t *h);	/* Add a http_pair_t objects to flow's http_pair_t chain */
		int extract_http(bool closed);	/* Extract http_pair_t objects from flow's packet_t chain */
		int add_packet(Packet *packet, register bool src);	/* Add a packet_t object to flow's packet_t chain */
};

#endif