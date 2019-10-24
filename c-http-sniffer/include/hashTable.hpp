#ifndef __HTABLE_H__
#define __HTABLE_H__

#include <pthread.h>
#include <vector>
#include <iostream>

//#include <sys/types.h>
//#include "attr.hpp"
#include "util.hpp"
#include "hashable.hpp"


using namespace std;

class Bucket;

class NodeHT
{
    public:
        Hashable* elem;
        NodeHT* next;
		NodeHT* prev;
	    Bucket	*bucket;

		// call delete on elem?
		~NodeHT();
};

class Bucket {
	public:
		NodeHT	*first;
		NodeHT	*last;
		pthread_mutex_t mutex;
		int		elm_cnt;
		Bucket();
};

class HashTable
{
	private:
		const int HASH_INIT_SIZE = 13200;
	public:
		// number of buckets in the ht
		pthread_mutex_t mutex;
		int size;
		// total number of item in the hash table
		int tot_cnt;
		// total number of updated item in the hash table
		// within the current interval (1 sec for rate computation)
		int int_cnt;
		vector<Bucket*> hashTable;

		HashTable();

		NodeHT* add(Hashable* value);
		NodeHT* find(Hashable* value);
		NodeHT* remove(Hashable* value);
		int insert(Hashable* value);
		int clear();
		//void reset();
		int getSize();
		int getCnt();
		int getSlotCnt();
		void print();
};

#endif
