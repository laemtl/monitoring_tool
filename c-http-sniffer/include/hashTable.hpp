#ifndef __HTABLE_H__
#define __HTABLE_H__

#include <pthread.h>
#include <sys/types.h>
#include "hash_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include <inttypes.h>
#include "util.h"
#include "hashable.hpp"

#define HASH_SIZE	13200

class Bucket;

class Node 
{
	public:
        Node	    *next;
        Node        *prev;
        Bucket	    *bucket;
        Hashable    *value;
        int         cnt;

    Node(Hashable* val) {
        value = val;
        cnt = 1;
    }
};

class Bucket {
	public:
        Node* first;
        Node* last;
        pthread_mutex_t mutex;
        int		elm_cnt;

    Bucket() {
        first = NULL;
        last = NULL;
        elm_cnt = 0;
        pthread_mutex_init(&mutex, NULL);
    }
};

class Hash
{
    public:
        // number of buckets in the ht
        pthread_mutex_t mutex;
        int size;
        // total number of item in the hash table
        int cnt;
        Bucket* buckets;
        
        Hash();
        Node* create(Hashable* value);
        Node* find(Hashable* value);
        Node* remove(Hashable* value);
        int add(Hashable* value);
        int clear();
        void reset();
        int getSize();
        int getCnt();
        int getScnt();
        void print();
};

#endif