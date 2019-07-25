#include "attr.h"

#ifndef PATH_H
#define PATH_H

unsigned long path_hash_fn(Attr* a);
int path_compare(Attr* a1, Attr* a2);

#endif
