#include <stdlib.h>
#include <inttypes.h>

#include "util.h"
#include "data.h"
#include "attr.h"

#include "top_list.h"

void tl_init(top_list* tl) {
    tl->size = TL_SIZE;
    tl->list = MALLOC(void*, tl->size);
    pthread_mutex_init(&(tl->mutex), NULL);
}

void tl_delete(top_list* tl) {
    int i;
    int count = tl->count;
    
    for(i = 0; i < count; i++) {
        free(tl->list[i]);
    }

    free(tl->list);
}

BOOL is_top(void** tl, int count, void* c, int (*compare_fn)(void*, void*)) {
    int i = 0;
    while (i < count) {
        if (compare_fn(c, tl[i]) == 0) return TRUE;
        i++;
    }
    return FALSE;
}

int sort_tl(void** tl, int n_index, int (*compare_fn)(void*, void*)) {
    if(n_index == 0) return;
    
    int i = n_index;
    // Element to sort is stored at the last position
    void* e = tl[i];
    if(e == NULL) return -1;
    
    while (i > 0 && compare_fn(e, tl[i-1]) > 0) {   
        tl[i] = tl[i-1];
        i = i - 1;
    }
    tl[i] = e; 

    // Return the position where inserted.
    return i;
}

void update_tl(Attr* a, hash_t* ht) {
    top_list* tl = &(ht->tl);
    // if the attr is already in the top list, return
    int count = tl->count;
    
    if(is_top(tl->list, count, (void*)a, ht->compare_fn)) return;

    pthread_mutex_lock(&(tl->mutex));
    // We have a spot left, add the attr in the list
    int size = ht->tl.size;
    
    if(count < 5) {
        tl->list[count] = MALLOC(Attr, 1);

        // Copy the element because it can be removed from the hash table
        memcpy(tl->list[count], a, sizeof(a));
        sort_tl(tl->list, count, req_compare);
        tl->count++;

        pthread_mutex_unlock(&(tl->mutex));
        return;
    }

    Attr* min = tl->list[size-1]; 

   
    if(min->cnt < a->cnt) {
        // Insert new element in top list
        // Copy the element because it can be removed from the hash table
        memcpy(tl->list[size-1], a, sizeof(a));
       
        // Sort top list
        sort_tl(tl->list, tl->size-1, req_compare);
    }

    pthread_mutex_unlock(&(tl->mutex));
}

void print_conn_tl(top_list* tl) {
    printf("Count : %d \n", tl->count);
    int i = 0;
    while (i < tl->count) {   
		Attr* attr = (Attr*)(tl->list[i]);
		Addr* addr = (Addr*)(attr->elem);
        char *saddr = ip_ntos(addr->ip);

        printf("IP: %s Port: %" PRIu16 "\n", saddr, addr->port);
        free(saddr);

        i++;
    }
}