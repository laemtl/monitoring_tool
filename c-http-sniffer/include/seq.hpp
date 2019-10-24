#ifndef __SEQ_H__
#define __SEQ_H__

#include "packet.hpp"
#include "tcpHeader.hpp" 

/* Sequence Time */

class Seq
{
	public:
		Packet 	*pkt;		/* packet */
		u_int32_t	seq;		/* seq order */
		u_int32_t	nxt_seq;	/* next in seq order */
		bool		ack;		/* acknowledged */
		u_int8_t	th_flags;	/* TCP flags of packet */
		time_t		cap_sec;
		time_t		cap_usec;
		bool		processed;
		u_int32_t	size;

		Seq		*next;		/* next in seq order */
		Seq();
		Seq::Seq(Packet *p);
		int compare(Seq *seq);
};

#endif