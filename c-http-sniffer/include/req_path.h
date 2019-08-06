#ifndef PATH_H
#define PATH_H

#include "attr.h"
#include "data.h"

unsigned long req_path_hash_fn(Attr* a);
int req_path_compare(Attr* a1, Attr* a2);
void req_path_cfl_add(char* req_path, int cnt, Result* r);

#endif
