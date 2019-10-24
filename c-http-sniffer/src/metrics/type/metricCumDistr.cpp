#include <stdlib.h>
#include <inttypes.h>
#include <iostream>

#include "util.hpp"
#include "attr.hpp"

#define CFL_SIZE 100
#define MIN_FREQ (double)1/CFL_SIZE

class MCDItem {
	public:
		char* name;
    	double freq;
};

// Vector ??
class MCDList {
	public:
		MCDItem list[CFL_SIZE];
		pthread_mutex_t mutex;
		int size;
		int count;

		MCDList();
		void add(char* name, double freq);
		void print();

};

MCDList::MCDList() 
	: size(CFL_SIZE), count(0)
{
    pthread_mutex_init(&mutex, NULL);
}

void MCDList::add(char* name, double freq) {
    pthread_mutex_lock(&mutex);
    if(count >= size) {
        pthread_mutex_unlock(&mutex);
        return;
    }
    list[count].name = name;
    list[count].freq = freq;
    count++;
    pthread_mutex_unlock(&mutex);
}

/*void MCDList::delete() {
    for(int i = 0; i < count; i++) {
        free(list[i].name);
    }
}*/

void MCDList::print() {
    for(int i = 0; i < count; i++) {
        printf("name: %s \n", list[i].name);
        printf("freq: %f \n", list[i].freq);
    }
}



class MetricCumDistr: public Metric
{
	private:
        int typeId;
		HashTable ht;
		cf_list cfl;

    public:
		MetricCumDistr();
		~MetricCumDistr();
		void print();
};

MetricCumDistr::~MetricCumDistr() {
	hash_clear(ht);
	hash_reset(ht);
}


void MetricCumDistr::print() {
	if(isActive())
		printf("%s cumulated freq list \n", name);
		print_cfl(&(cfl));
		printf("\n");
}

void extract_freq_ht(hash_t* ht, Result* r, void (*cfl_add_fn)(void*, int, Result*)) {
	int i;
	//hash_mb_t *hm = NULL;
	//node	*e = NULL;

	for(i=0; i<ht->size; i++){
		pthread_mutex_lock(&(ht->buckets[i]->mutex));
	}

	for(i=0; i<ht->size; i++){
		node* n = ht->buckets[i]->first;
		while(n != NULL) {
			Attr* a = (Attr*) n->value;
			cfl_add_fn(a->elem, a->cnt, r);
			n = n->next;
		}
	}

	for(i=0; i<ht->size; i++){
		pthread_mutex_unlock(&(ht->buckets[i]->mutex));
	}
}

void extract_freq_ar(int* ar, int size, Result* r, void (*cfl_add_fn)(int, int, Result*)) {
	int i;
	for(i=0; i<size; i++){
		cfl_add_fn(i, ar[i], r);
	}
}

/* 
	int c = result->client.count;
	client = MALLOC(Analysis__Freq*, c); 
	
	for(int i = 0; i < c2; i++) {
		client[j] = MALLOC(Analysis__Freq, 1);
		analysis__freq__init(client[j]);
		client[j]->name = result->client.list[j].name;
		client[j]->freq = result->client.list[j].c_freq;
	}
	msg.n_client = c;
	msg.client = client;


		for(j = 0; j < c2; j++) free(client[j]);
		free(client);
*/