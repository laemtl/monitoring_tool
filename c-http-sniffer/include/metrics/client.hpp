#ifndef __CLIENTS_H__
#define __CLIENTS_H__

#include <sys/types.h>
#include "hashTable.hpp"
#include "result.hpp"

u_int32_t client_hash_fn(Attr* a);
int ip_compare(Attr* c1, Attr* c2);
void client_cfl_add(void* addr, int cnt, Result* r);
void add_client(u_int32_t saddr, Data* data);
BOOL is_client_ht(hash_t* ht);

#endif
