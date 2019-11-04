#ifndef __HASHABLE_H__
#define __HASHABLE_H__

#include <sys/types.h>
#include <cstdlib>

class Hashable 
{
	public:
		void* value;
		pthread_mutex_t mutex;
        
        Hashable(void* val) : value(val) {};
        
        void setValue(void* val) {
            value = val;
        }

        ~Hashable() {
            free(value);
        }

        virtual u_int32_t hash() = 0;
        virtual int compare(Hashable* elem) = 0;       
};

#endif