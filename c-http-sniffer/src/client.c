#include "client.h";
#include "data.h";
#include <inttypes.h>


extern int client_hash_fn(Client* c) {
    Data* data = {0};
	get_data(&data);
    return ((c->addr.port & 0xff) | ((c->addr.ip & 0xff) << 8)) % data->client_ht.size;
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
    Data* data = {0};
	get_data(&data);
    
    pthread_mutex_lock(&(c->mutex));
	c->req_tot++;

    if(c->stamp < data->stamp) {
        c->stamp = data->stamp; 
        data->client_ht.sub_cnt++;
    } 
	pthread_mutex_unlock(&(c->mutex));    
    
    // if the client is already in the top list, return
    int count = data->client_tl.count;
    
    if(is_top(data->client_tl.top_list, count, (void*)c, addr_compare)) return;

    //printf("Not already in top \n");

    pthread_mutex_lock(&(data->client_tl.mutex));
    
    // We have a spot left, add the client in the list
    int size = data->client_tl.size;
    
    if(count < 5) {
        data->client_tl.top_list[count] = MALLOC(Client, 1);
        memcpy(data->client_tl.top_list[count], c, sizeof(c));
                
        //c->is_top = TRUE;
        sort_tl(data->client_tl.top_list, count-1, req_compare);
        // TODO: need a lock
        (&(data->client_tl))->count++;

        pthread_mutex_unlock(&(data->client_tl.mutex));
        return;
    }

    Client* min = data->client_tl.top_list[size-1]; 

    //printf("pointer of min client: %p \n", data->client_tl.top_list[size-1]);
   
    if(min->req_tot < c->req_tot) {

        // Remove last element from top list
        //((Client*)data->client_tl.top_list[size-1])->is_top = FALSE;
        
        // Insert new element in top list
        memcpy(data->client_tl.top_list[size-1], c, sizeof(c));
        //data->client_tl.top_list[size-1] = c;
        //c->is_top = TRUE;

        // Sort top list
        sort_tl(data->client_tl.top_list, data->client_tl.size-1, req_compare);

        pthread_mutex_unlock(&(data->client_tl.mutex));
        return;
    }

    pthread_mutex_unlock(&(data->client_tl.mutex));
}

void client_init(Data* data) {
    if(data == NULL) return;

    //data->clients_ht = MALLOC(void*, HASH_SIZE);
    data->client_ht.size = HASH_SIZE;
    data->client_tl.top_list = MALLOC(void*, data->client_tl.size);

    hash_init(&(data->client_ht));
    data->client_ht.hash_fn = &client_hash_fn;
    data->client_ht.compare_fn = &addr_compare;
    data->client_ht.update_fn = &client_update;

    pthread_mutex_init(&(data->client_tl.mutex), NULL);
};