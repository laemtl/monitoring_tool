#ifndef CLIENTS_H
#define CLIENTS_H

#include <sys/types.h>
#include "hash_table.h"
#include "result.h"
#include "attr.h"

typedef struct _addr Addr;
struct _addr {
	u_int32_t	ip;
	u_int16_t	port;
};

#include "data.h"

u_int32_t client_hash_fn(Attr* a);
int ip_compare(Attr* a1, Attr* a2);
void client_cfl_add(u_int32_t* addr, int cnt, Result* r);

#endif
