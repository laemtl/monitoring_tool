#ifndef __ORDER_H__
#define __ORDER_H__

#include <sys/types.h>

#include "packet.hpp"
#include "seq.hpp"

//#include "util.hpp"

/* data to order a TCP stream */
class Order 
{
	public:
		Seq		*src;		/* source packet list ordered */
		Seq		*dst;		/* destination packet list ordered */
		Seq		*last_src;	/* last in src queue inserted */
		Seq		*last_dst;	/* last in dst queue inserted */
		u_int32_t	num;		/* number of packet in queue */
		bool		rst;		/* reset */
		//pthread_mutex_t mutex;
	
		Order();
		~Order();
};

#endif