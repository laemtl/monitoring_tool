#ifndef __ATTR_H__
#define __ATTR_H__

#include <pthread.h>
#include <inttypes.h>
#include <cstdlib>

#include "util.hpp"
#include "hashable.hpp"

class Attr : public Hashable {

    private: 
        void *elem;
        int cnt;
        pthread_mutex_t mutex;
    
    public:
        Attr(void* e);
        ~Attr();
        void update();
        void add();
        int compare(Attr* a);
};

#endif