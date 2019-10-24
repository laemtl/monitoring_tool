#ifndef __CONN_H__
#define __CONN_H__

#include <sys/types.h>
#include "hashTable.hpp"
#include "util.hpp"
#include "attr.hpp"


u_int32_t conn_hash_fn(Attr* c);
int addr_compare(Attr* c1, Attr* c2);
void add_conn(u_int32_t saddr, u_int16_t sport, Data* data);
BOOL is_conn_ht(hash_t* ht);

#endif
