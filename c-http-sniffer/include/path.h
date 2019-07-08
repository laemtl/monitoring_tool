#include <sys/types.h>
#include <stdbool.h>
#include "hash_table.h"

#ifndef PATH_H
#define PATH_H

typedef struct _path Path;
struct _path {
	char* path;
	int	req_tot;
    pthread_mutex_t mutex;
};

#endif