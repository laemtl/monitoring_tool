#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include "util.h"
#include "cf_list.h"

void cfl_init(cf_list* cfl) {
    cfl->size = CFL_SIZE;
    cfl->count = 0;
    pthread_mutex_init(&(cfl->mutex), NULL);
}

void cfl_add(char* name, double c_freq, cf_list* cfl) {
    pthread_mutex_lock(&(cfl->mutex));
    if(cfl->count >= cfl->size) {
        pthread_mutex_unlock(&(cfl->mutex));
        return;
    }
    cfl->list[cfl->count].name = name;
    cfl->list[cfl->count].c_freq = c_freq;
    cfl->count++;
    pthread_mutex_unlock(&(cfl->mutex));
}

void cfl_delete(cf_list* cfl) {
    for(int i = 0; i < cfl->count; i++) {
        free(cfl->list[i].name);
        cfl->list[i].name = NULL;
        cfl->list[i].c_freq = 0;
    }
    cfl->count = 0;
}

void print_cfl(cf_list* cfl) {
    for(int i = 0; i < cfl->count; i++) {
        printf("name: %s \n", cfl->list[i].name);
        printf("freq: %f \n", cfl->list[i].c_freq);
    }
}