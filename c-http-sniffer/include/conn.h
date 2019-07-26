#include <sys/types.h>
#include "hash_table.h"

#ifndef CONN_H
#define CONN_H

#include "attr.h"

int conn_hash_fn(Attr* a);
int addr_compare(Attr* a1, Attr* a2);

#endif
