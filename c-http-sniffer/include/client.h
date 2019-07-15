#include <sys/types.h>
#include <stdbool.h>
#include "hash_table.h"

#ifndef CLIENTS_H
#define CLIENTS_H

typedef struct _addr Addr;
struct _addr {
	u_int32_t	ip;
	u_int16_t	port;
};

typedef struct _client Client;
struct _client {
	Addr addr;
	int	req_tot;
	int stamp;
    pthread_mutex_t mutex;
};

#endif