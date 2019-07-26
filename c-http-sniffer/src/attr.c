#include "attr.h"
#include "data.h"

Attr* attr_create(){
    Attr* a = CALLOC(Attr, 1);
	pthread_mutex_init(&(a->mutex), NULL);
    return a;
}

void update_attr(Attr* a, hash_t* ht) {
    Data* data = {0};
	get_data(&data);
    
    pthread_mutex_lock(&(a->mutex));
	a->req_tot++;

    // If attr stamp is smaller than current stamp
    // the attr was not detected for the current interval yet
    // Increment the subtotal count 
    if(a->stamp < data->stamp) {
        a->stamp = data->stamp; 
        pthread_mutex_unlock(&(a->mutex)); 

        pthread_mutex_lock(&(ht->mutex));
        ht->int_cnt++;
        pthread_mutex_unlock(&(ht->mutex));
        
    } else {
        pthread_mutex_unlock(&(a->mutex)); 
    }
	  

    update_tl(a, ht); 
}


void add_attr(void* e, hash_t* ht) {
    Data* data = {0};
	get_data(&data);
    
    Attr* attr = attr_create();
    attr->elem = e;
	attr->stamp = data->stamp;
    
	hash_add(attr, ht);
}


int req_compare(Attr* a1, Attr* a2) {
    if(a1 == NULL || a2 == NULL) {
        printf("Uninitilized value \n");
        return;
    }

    if(a1->req_tot == a2->req_tot) return 0;
    else if (a1->req_tot > a2->req_tot) return 1;
    return -1;
}
