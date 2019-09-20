#include <sys/types.h>
#include "hash_table.h"
#include "util.h"
#include "data.h"

#ifndef CONN_H
#define CONN_H

#include "attr.h"

u_int32_t conn_hash_fn(Attr* c);
int addr_compare(Attr* c1, Attr* c2);
void add_conn(u_int32_t saddr, u_int16_t sport, Data* data);
BOOL is_conn_ht(hash_t* ht);

#endif
