#ifndef __CF_LIST_H__
#define __CF_LIST_H__

#include <pthread.h>
#define CFL_SIZE 100
#define MIN_FREQ (double)1/CFL_SIZE

typedef struct _cf_item cf_item;
struct _cf_item {
	char* name;
    double c_freq;
};

typedef struct _cf_list cf_list;
struct _cf_list {
	cf_item list[CFL_SIZE] = {0};
    pthread_mutex_t mutex;
    int size;
    int count;
};

void cfl_init(cf_list* cfl);
void cfl_add(char* name, double c_freq, cf_list* cfl);
void cfl_delete(cf_list* cfl);
void print_cfl(cf_list* cfl);

#endif
