#include "path.h";
#include "data.h";
#include <inttypes.h>

unsigned long str_hash_fn(unsigned char *str) {
    Data* data = {0};
	get_data(&data);
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c;

    return hash % data->path_ht.size;
}

extern int path_compare(Path* p1, Path* p2) {
    if(p1->path > p2->path) return 1;
    else if(p1->path < p2->path) return 1;
    return 0;
}

extern void path_update(Path* p) {
    int n;

    pthread_mutex_lock(&(p->mutex));
	p->req_tot++;
	pthread_mutex_unlock(&(p->mutex));    
    
    // if the client is already in the top list, return
    Data* data = {0};
	get_data(&data);
    int count = data->path_tl.count;
    
    if(is_top(data->path_tl.top_list, count, (void*)p, path_compare)) return;

    //printf("Not already in top \n");

    pthread_mutex_lock(&(data->path_tl.mutex));
    
    // We have a spot left, add the client in the list
    int size = data->path_tl.size;
    
    if(count < 5) {
        data->path_tl.top_list[count] = MALLOC(Client, 1);
        memcpy(data->path_tl.top_list[count], p, sizeof(p));
                
        //c->is_top = TRUE;
        sort_tl(data->path_tl.top_list, count-1, path_compare);
        // TODO: need a lock
        (&(data->path_tl))->count++;

        pthread_mutex_unlock(&(data->path_tl.mutex));
        return;
    }

    Path* min = data->path_tl.top_list[size-1]; 

   
    if(min->req_tot < p->req_tot) {

        // Remove last element from top list
        //((Client*)data->clients_tl.top_list[size-1])->is_top = FALSE;
        
        // Insert new element in top list
        memcpy(data->path_tl.top_list[size-1], p, sizeof(p));
        //data->clients_tl.top_list[size-1] = c;
        //c->is_top = TRUE;

        // Sort top list
        sort_tl(data->path_tl.top_list, data->path_tl.size-1, path_compare);

        pthread_mutex_unlock(&(data->path_tl.mutex));
        return;
    }

    pthread_mutex_unlock(&(data->path_tl.mutex));
}

void path_init(Data* data) {
    if(data == NULL) return;

    //data->clients_ht = MALLOC(void*, HASH_SIZE);
    data->path_ht.size = HASH_SIZE;
    data->path_tl.top_list = MALLOC(void*, data->path_tl.size);

    hash_init(&(data->path_ht));
    data->path_ht.hash_fn = &str_hash_fn;
    data->path_ht.compare_fn = &path_compare;
    data->path_ht.update_fn = &path_update;

    pthread_mutex_init(&(data->path_tl.mutex), NULL);
};