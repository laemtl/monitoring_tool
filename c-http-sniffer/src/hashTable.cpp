#include "hashTable.hpp"

Hashable::Hashable(void* val) : value(val) {
}

void Hashable::setValue(void* val) {
    value = val;
}

int Hashable::compare(Hashable* elem) {
    if(value == elem->value) return 0;
    else if (value > elem->value) return 1;
    return -1;
}

u_int32_t Hashable::hash() {
    return *(u_int32_t*)value;
}

Hashable::~Hashable() {
    free(value);
}

/* Initiate the hash table with no elems */
Hash::Hash() {
    pthread_mutex_init(&mutex, NULL);
    size = HASH_SIZE;
    buckets = new Bucket[size];
    /*for(int i=0; i<size; i++){
        buckets[i] = new Bucket();
        buckets[i]->first = NULL;
        buckets[i]->last = NULL;
        buckets[i]->elm_cnt = 0;
        pthread_mutex_init(&(buckets[i]->mutex), NULL);
    }*/
}

/* Create a new record in hash table */
Node* Hash::create(Hashable *value) {
    if(value == NULL) error("Value can't be NULL \n");
    
    Node* n = new Node(value);
    Bucket b = buckets[value->hash() % size];

    pthread_mutex_lock(&(b.mutex));

    if(b.elm_cnt == 0 ){
        n->next = NULL;
        n->prev = NULL;
        b.first = n;
    } else {
        b.last->next = n;
        n->prev = b.last;
    }
    b.last = n;
    b.last->next = NULL;
    n->bucket = &b;
    b.elm_cnt++;
    cnt++;

    pthread_mutex_unlock(&(b.mutex));
    return n;
}

/* Try to find a record in hash table */
Node* Hash::find(Hashable *value) {
    if(value == NULL) error("Value can't be NULL \n");
    
    u_int32_t key = value->hash() % size;
    Bucket b = buckets[key];
    Node	*e = NULL;

    pthread_mutex_lock(&(b.mutex));

    if (b.elm_cnt > 0){
        e = b.first;
        while(e != NULL){
            if(value->compare(e->value) == 0){
                pthread_mutex_unlock(&(b.mutex));
                return e;
            } else {
                e = e->next;
                continue;
            }
        }
    }
    pthread_mutex_unlock(&(b.mutex));
    return NULL;
}

/* Delete a record in hash table */
Node* Hash::remove(Hashable *value) {
    if(value == NULL) error("Value can't be NULL \n");

    Node* n = find(value);
    if(n == NULL) return NULL;

    Bucket *b = n->bucket;

    if( b->elm_cnt == 1 && n == b->first){
        b->first = NULL;
        b->last = NULL;
    } else {
        if(n->prev == NULL){	/* the first record */
            b->first = n->next;
            b->first->prev = NULL;
        } else if(n->next == NULL) {
            b->last = n->prev;
            b->last->next = NULL;
        } else {
            n->prev->next = n->next;
            n->next->prev = n->prev;
        }
    }
    n->next = NULL;
    n->prev = NULL;
    b->elm_cnt--;
    cnt--;
    return n;
}

/*
* Add an element to hash table
* Update the element if the element already exists;
* Otherwise, make a new record and add the value to it.
*/

int Hash::add(Hashable* value) {
    if(value == NULL) error("Value can't be NULL \n");

    Node* e = NULL;
    e = find(value);
    if(e == NULL) {
        e = create(value);
        return 0;
    } else {
        e->cnt++;
        return 1;
    }    
}

/* Clear the flow hash table thoroughly */
int Hash::clear() {
    int i;
    for(i=0; i<size; i++){
        pthread_mutex_lock(&(buckets[i].mutex));
    }

    Node* elem = NULL;
    cnt = 0;
    for(i=0; i<size; i++){
        while(buckets[i].first != NULL ){
            elem = buckets[i].first;
            buckets[i].first = buckets[i].first->next;
            delete elem->value;
            delete elem;
            buckets[i].elm_cnt--;
        }
        buckets[i].first = NULL;
        buckets[i].last = NULL;
    }

    for(i=0; i<size; i++){
        pthread_mutex_unlock(&(buckets[i].mutex));
    }

    return 0;
}

void Hash::reset() {
    int i;
    /*for(i=0; i<size; i++){
        free(buckets[i]);
    }*/
    delete[] buckets;
}

/* Return the size of hash table */
int Hash::getSize() {
    return size;
}

/* Return the flow count of hash table */
int Hash::getCnt() {
    return cnt;
}

/* Return the count of hash slots consumed */
int Hash::getScnt() {
    int i = 0;
    int slot_consumed = 0;
    
    for(i=0; i<size; i++)
    {
        pthread_mutex_lock(&(buckets[i].mutex));
    }
    
    for(i=0; i<size; i++)
    {
        if(buckets[i].first != NULL)
        {
            slot_consumed++;
        }
    }
    
    for(i=0; i<size; i++)
    {
        pthread_mutex_unlock(&(buckets[i].mutex));
    }
    return slot_consumed;
}

/* Print hash table details for debugging */
void Hash::print() {
    printf("(Hash size)%d : (Consumed)%d : (Flows)%d\n", getSize(), getScnt(), getCnt() );
}