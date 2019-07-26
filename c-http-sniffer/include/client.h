#include <sys/types.h>
#include "hash_table.h"

#ifndef CLIENTS_H
#define CLIENTS_H

#include "attr.h"

typedef struct _addr Addr;
struct _addr {
	u_int32_t	ip;
	u_int16_t	port;
};

/*typedef struct _client Client;
struct _client {
	Addr addr;
	int	req_tot;
	int stamp;
    pthread_mutex_t mutex;
};*/

u_int32_t client_hash_fn(Attr* a);
int ip_compare(Attr* a1, Attr* a2);

#endif
