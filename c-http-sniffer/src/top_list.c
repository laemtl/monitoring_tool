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
    // Element to insert is stored at the last position
    void* e = tl[i];
    if(e == NULL) printf("e is NULL \n");
    if(tl[i-1] == NULL) printf("tl is NULL \n");
    
    while (i > 0 && compare_fn(e, tl[i-1]) > 0) {   
        tl[i] = tl[i-1];
        i = i - 1;

        if(e == NULL) printf("e is NULL \n");
        if(tl[i-1] == NULL) printf("tl is NULL \n");
    }
    tl[i] = e; 

    // Return the position where inserted.
    return i;
}

void update_tl(Attr* elem, hash_t* ht) {
    top_list* tl = &(ht->tl);

    // if the attr is already in the top list, return
    int count = tl->count;
    if(is_top(tl->list, count, (void*)elem, ht->compare_fn)) return;
    pthread_mutex_lock(&(tl->mutex));
    // We have a spot left, add the attr in the list
    int size = ht->tl.size;
    
    if(count < 5) {
        tl->list[count] = MALLOC(Attr, 1);

        // Copy the element because it can be removed from the hash table
        memcpy(tl->list[count], elem, sizeof(elem));
        sort_tl(tl->list, count, req_compare);
        tl->count++;

        pthread_mutex_unlock(&(tl->mutex));
        return;
    }

    Attr* min = tl->list[size-1]; 

   
    if(min->req_tot < elem->req_tot) {
        // Insert new element in top list
        // Copy the element because it can be removed from the hash table
        memcpy(tl->list[size-1], elem, sizeof(elem));
       
        // Sort top list
        sort_tl(tl->list, tl->size-1, req_compare);

        pthread_mutex_unlock(&(tl->mutex));
        return;
    }

    pthread_mutex_unlock(&(tl->mutex));
}

void print_conn_tl(top_list* tl) {
    printf("Count : %d \n", tl->count);
    int i = 0;
    while (i < tl->count) {   
        int n = sizeof("aaa.bbb.ccc.ddd") + 1;
        char *saddr[n];
		Attr* attr = (Attr*)(tl->list[i]);
		Addr* addr = (Addr*)(attr->elem);
        strncpy(saddr, ip_ntos(addr->ip), n);
        saddr[n] = '\0';

        printf("IP: %s Port: %" PRIu16 "\n", saddr, addr->port);
        i++;
    }
}

void print_client_tl(top_list* tl) {
    printf("Count : %d \n", tl->count);
    int i = 0;
    while (i < tl->count) {   
        int n = sizeof("aaa.bbb.ccc.ddd") + 1;
        char *saddr[n];
		Attr* attr = (Attr*)(tl->list[i]);
		Addr* addr = (Addr*)(attr->elem);
        strncpy(saddr, ip_ntos(addr->ip), n);
        saddr[n] = '\0';

        printf("IP: %s \n", saddr, addr->port);
        i++;
    }
}

void print_path_tl(top_list* tl) {
    printf("Count : %d \n", tl->count);
    int i = 0;
    while (i < tl->count) {   
        Attr* attr = (Attr*)(tl->list[i]);
		char* path = (char*)(attr->elem);
        printf("path: %s \n", path);
        i++;
    }
}

void clear_tl() {

}