#ifndef __ADDR_H__
#define __ADDR_H__

#include <sys/types.h>

/*class Addr {
    public:
        u_int32_t	ip;
	    u_int16_t	port;
};*/

typedef struct _addr Addr;
struct _addr {
	u_int32_t	ip;
	u_int16_t	port;
};

#endif