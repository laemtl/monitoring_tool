#ifndef PATH_H
#define PATH_H

#include "attr.h"
#include "analysis.hpp"

u_int32_t req_path_hash_fn(Attr* a);
int req_path_compare(Attr* a1, Attr* a2);
char* extract_req_path(const char* uri);
char* extract_req_query(const char* uri);
void req_path_cfl_add(void* req_path, int cnt, Result* r);
void add_req_path(const char *uri, Data* data);
BOOL is_path_ht(hash_t* ht);

#endif
