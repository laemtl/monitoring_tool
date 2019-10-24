#ifndef __FLOWSOCKET_H__
#define __FLOWSOCKET_H__

#include <sys/types.h>
#include <cstring>

class FlowSocket
{
	public:
		u_int32_t	saddr;
		u_int32_t	daddr;
		u_int16_t	sport;
		u_int16_t	dport;

        int compare(FlowSocket *fs);
		u_int32_t hash(int size);
};

#endif
