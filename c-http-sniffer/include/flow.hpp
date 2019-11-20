#ifndef FLOW_H_
#define FLOW_H_

#include <pthread.h>
#include <sys/types.h>

#include "packet.h"
#include "order.h"
#include "http.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <assert.h>
#include <sys/time.h>
#include <unistd.h>

#include "util.h"
#include "tcp.h"

#include "queue.hpp"

#define CLIENT_CLOSE	0x01
#define SERVER_CLOSE	0x02
#define FORCED_CLOSE	0x04


/**
 * flow socket
 */
typedef struct _flow_s	flow_s;
struct _flow_s
{
	u_int32_t	saddr;
	u_int32_t	daddr;
	u_int16_t	sport;
	u_int16_t	dport;
};

/**
 * Hash management block
 */

class Flow;
class FlowHashTable;

typedef struct _hash_mb_t hash_mb_t;

struct _hash_mb_t
{
	Flow	*first;
	Flow	*last;
	pthread_mutex_t mutex;
	int		elm_cnt;
};

class Flow {
	public:
        /**
         * Extendable flow structure which stores src/dst packet queues.
         */
            Flow		*next;
            Flow		*prev;
        /* Packets of flow */
            packet_t	*pkt_src_f;		/* front of packets src queue */
            packet_t	*pkt_src_e;		/* end of packets src queue */
            packet_t	*pkt_dst_f;		/* front of packets dst queue */
            packet_t	*pkt_dst_e;		/* end of packets dst queue */
            u_int32_t	pkt_src_n;		/* number of packet in src queue */
            u_int32_t	pkt_dst_n;		/* number of packet in dst queue */
            u_int32_t	pkts_src;       /* total packets count of the flow sent by src */
            u_int32_t	pkts_dst;	    /* total packets count of the flow sent by dst */
            order_t		*order;		    /* info to order the packet */
            hash_mb_t	*hmb;		    /* hash management block */
        /* TCP info */
            flow_s		socket;		    /* flow socket with 4 tuple */
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
            pair_t		*pair_f;	    /* front of pair queue */
            pair_t		*pair_e;	    /* end of pair queue */
        /* Control */
            time_t		last_action_sec;	/* latest modified time to the flow in seconds */
            time_t		last_action_usec;	/* latest modified time to the flow in microseconds */
            u_int8_t	close;
            bool		init;			/* the flow has received its first packet */
            bool 		processed;

            Queue* fq;
            FlowHashTable* fht;


		Flow(Queue* fq, FlowHashTable* fht);
        ~Flow();
		void reset();
		int hook_packet(packet_t *packet, bool src);
		int cal_packet(packet_t *packet, bool src);
		int compare_sequence_time(seq_t *seq1, seq_t *seq2);
		int flow_socket_cmp(flow_s *fs);
		int add_http(pair_t *h);
		int add_packet(packet_t *packet, register bool src);
		int extract_http(bool closed);
};

#include "flowHashTable.hpp"

#endif /* FLOW_H_ */