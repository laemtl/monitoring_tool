#ifndef CLIENTS_H
#define CLIENTS_H

#include <sys/types.h>
#include "hash_table.h"
#include "result.h"
#include "data.h"

u_int32_t client_hash_fn(Attr* a);
int ip_compare(Attr* c1, Attr* c2);
void client_cfl_add(void* addr, int cnt, Result* r);
void add_client(u_int32_t saddr, Data* data);
BOOL is_client_ht(hash_t* ht);

#endif
