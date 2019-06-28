#include "clients.h";
#include "data.h";


extern int client_hash_fn(Client* c) {
    Data* data = {0};
	get_data(&data);
    return ((c->addr.port & 0xff) | ((c->addr.ip & 0xff) << 8)) % data->clients_ht->size;
}

extern int client_compare(Client* c1, Client* c2) {
    if((c1->addr.ip == c2->addr.ip) && (c1->addr.port == c2->addr.port)) return 0;
    return 1;
}

extern void client_update(Client* c) {
    /*int i, n;
    printf("Beginning of update \n");

    // if the client is already in the top list, return
    if(c->is_top) return;

    Data* data = {0};
	get_data(&data);

    pthread_mutex_lock(&(c->mutex));
	c->req_tot += 1;
	pthread_mutex_unlock(&(c->mutex));
    
    n = data->clients_top_size;
    pthread_mutex_lock(&(data->ct_mutex));

	for(i = 0; i<n; i++) {
        printf("i: %d \n", i);
        printf("client pointer: %p \n", c);
    */
        /*if(
            data->clients_top[i] == NULL 
            || c->req_tot > data->clients_top[i]->req_tot
        ) {*/

        // we have a spot left, add the client in the list
    /*  if(data->clients_top[i] == NULL) {
            data->clients_top[i] = c;
            pthread_mutex_unlock(&(data->ct_mutex));
            return;
        }

        int tot1 = c->req_tot;
        printf("pointer of client_top at i: %p \n", data->clients_top[i]);
        int tot2 = data->clients_top[i]->req_tot;

        // the client is a new top client
        if(tot1 > tot2) {
            data->clients_top[i] = c;
            pthread_mutex_unlock(&(data->ct_mutex));
            return;
        }

        // sort_array()
// compare and insert on the last slot (replace min)
// sort the inserted element
    }

    pthread_mutex_unlock(&(data->ct_mutex));
    */
}

void client_init(Data* data) {
    if(data == NULL) return;
    
    data->clients_ht = MALLOC(hash_t, 1);
    data->clients_top= MALLOC(Client*, data->clients_top_size);

    hash_init(data->clients_ht, HASH_SIZE);
    data->clients_ht->hash_fn = &client_hash_fn;
    data->clients_ht->compare_fn = &client_compare;
    data->clients_ht->update_fn = &client_update;

    pthread_mutex_init(&(data->ct_mutex), NULL);
};