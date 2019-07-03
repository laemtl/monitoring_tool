#include "clients.h";
#include "data.h";
#include <inttypes.h>

bool is_top(void** tl, int count, void* c, int (*compare_fn)(void*, void*)) {
    int i = 0;
    while (i < count) {
        if (compare_fn(c, tl[i]) == 0) return TRUE;
        i++;
    }
    return FALSE;
}

extern int client_hash_fn(Client* c) {
    Data* data = {0};
	get_data(&data);
    return ((c->addr.port & 0xff) | ((c->addr.ip & 0xff) << 8)) % data->clients_ht.size;
}

extern int addr_compare(Client* c1, Client* c2) {
    if((c1->addr.ip == c2->addr.ip) && (c1->addr.port == c2->addr.port)) return 0;
    return 1;
}

extern int req_compare(Client* c1, Client* c2) {
    if(c1->req_tot == c2->req_tot) return 0;
    else if (c1->req_tot > c2->req_tot) return 1;
    return -1;
}

extern void client_update(Client* c) {
    int n;

    pthread_mutex_lock(&(c->mutex));
	c->req_tot++;
	pthread_mutex_unlock(&(c->mutex));    
    
    // if the client is already in the top list, return
    Data* data = {0};
	get_data(&data);
    int count = data->clients_tl.count;
    
    if(is_top(data->clients_tl.top_list, count, (void*)c, addr_compare)) return;

    //printf("Not already in top \n");

    pthread_mutex_lock(&(data->clients_tl.mutex));
    
    // We have a spot left, add the client in the list
    int size = data->clients_tl.size;
    
    if(count < 5) {
        data->clients_tl.top_list[count] = MALLOC(Client, 1);
        memcpy(data->clients_tl.top_list[count], c, sizeof(c));
                
        //c->is_top = TRUE;
        sort_tl(data->clients_tl.top_list, count-1, req_compare);
        // TODO: need a lock
        (&(data->clients_tl))->count++;

        pthread_mutex_unlock(&(data->clients_tl.mutex));
        return;
    }

    Client* min = data->clients_tl.top_list[size-1]; 

    printf("pointer of min client: %p \n", data->clients_tl.top_list[size-1]);
   
    if(min->req_tot < c->req_tot) {

        // Remove last element from top list
        //((Client*)data->clients_tl.top_list[size-1])->is_top = FALSE;
        
        // Insert new element in top list
        memcpy(data->clients_tl.top_list[size-1], c, sizeof(c));
        //data->clients_tl.top_list[size-1] = c;
        //c->is_top = TRUE;

        // Sort top list
        sort_tl(data->clients_tl.top_list, data->clients_tl.size-1, req_compare);

        pthread_mutex_unlock(&(data->clients_tl.mutex));
        return;
    }

    pthread_mutex_unlock(&(data->clients_tl.mutex));
}

void client_init(Data* data) {
    if(data == NULL) return;

    //data->clients_ht = MALLOC(void*, HASH_SIZE);
    data->clients_ht.size = HASH_SIZE;
    data->clients_tl.top_list = MALLOC(void*, data->clients_tl.size);

    hash_init(&(data->clients_ht));
    data->clients_ht.hash_fn = &client_hash_fn;
    data->clients_ht.compare_fn = &addr_compare;
    data->clients_ht.update_fn = &client_update;

    pthread_mutex_init(&(data->clients_tl.mutex), NULL);
};

int sort_tl(void** tl, int n_index, int (*compare_fn)(void*, void*)) {
    //if(n_index == 0) return;
    
    int i = n_index;
    // Element to insert is stored at the last position
    void* e = tl[i];
    while (i > 0 && compare_fn(e, tl[i-1]) > 0) {   
        tl[i] = tl[i-1];
        i = i - 1;
    }
    tl[i] = e; 

    // Return the position where inserted.
    return i;
}