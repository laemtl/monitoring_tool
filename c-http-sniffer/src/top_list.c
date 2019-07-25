#include <stdlib.h>

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
    top_list tl = ht->tl;

    // if the client is already in the top list, return
    int count = tl.count;
    if(is_top(tl.list, count, (void*)elem, ht->compare_fn)) return;
    pthread_mutex_lock(&(tl.mutex));
    // We have a spot left, add the client in the list
    int size = ht->tl.size;
    
    if(count < 5) {
        tl.list[count] = MALLOC(Attr, 1);
        memcpy(tl.list[count], elem, sizeof(elem));
        sort_tl(tl.list, count, req_compare);
        // TODO: need a lock
        tl.count++;

        pthread_mutex_unlock(&(tl.mutex));
        return;
    }

    Attr* min = tl.list[size-1]; 

   
    if(min->req_tot < elem->req_tot) {

        // Remove last element from top list
        //((Client*)data->clients_tl.list[size-1])->is_top = FALSE;
        
        // Insert new element in top list
        memcpy(tl.list[size-1], elem, sizeof(elem));
        //data->clients_tl.list[size-1] = c;
        //c->is_top = TRUE;

        // Sort top list
        sort_tl(tl.list, tl.size-1, req_compare);

        pthread_mutex_unlock(&(tl.mutex));
        return;
    }

    pthread_mutex_unlock(&(tl.mutex));
}