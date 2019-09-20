#include "attr.h"
#include "data.h"
#include <inttypes.h>

Attr* create_attr(void* e){
    Data* data = {0};
	get_data(&data);
    
    Attr* a = CALLOC(Attr, 1);
    a->elem = e;
	a->stamp = data->stamp;
    pthread_mutex_init(&(a->mutex), NULL);
    
    return a;
}

void delete_attr(Attr* a){
    free(a->elem);
    free(a);
}

void update_attr(Attr* a, hash_t* ht) {
    Data* data = {0};
	get_data(&data);
    
    pthread_mutex_lock(&(a->mutex));
	a->cnt++;

    // If attr stamp is smaller than current stamp
    // the attr was not detected for the current interval yet
    // Increment the subtotal count 
    if(a->stamp < data->stamp) {
        a->stamp = data->stamp; 
       
        /*pthread_mutex_lock(&(ht->mutex));
        ht->int_cnt++;
        pthread_mutex_unlock(&(ht->mutex));*/
        
    }

    pthread_mutex_unlock(&(a->mutex));   

    //if(is_conn_ht(ht)) update_tl(a, ht); 
}

void add_attr(void* e, hash_t* ht) {
    Attr* attr = create_attr(e);
	hash_add(attr, ht);
}

int req_compare(Attr* a1, Attr* a2) {
    if(a1 == NULL || a2 == NULL) {
        error("Uninitilized value \n");
    }

    if(a1->cnt == a2->cnt) return 0;
    else if (a1->cnt > a2->cnt) return 1;
    return -1;
}
