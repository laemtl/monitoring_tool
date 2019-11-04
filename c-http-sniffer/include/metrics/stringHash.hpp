#ifndef __STRING_HASH_H__
#define __STRING_HASH_H__

#define MAGIC_HASH_NUMBER 5381
#include "hashable.hpp"

class StringHash : public Hashable
{
	public: 
		StringHash(char* val) : Hashable((void*)val) {};

        // djb2 has excellent distribution and speed
		u_int32_t hash() {
            char *str = (char*)value;

            int c;
            unsigned long hash = MAGIC_HASH_NUMBER;
            while ((c = *str++))
                    hash = ((hash << 5) + hash) + c;

            if(hash < 0) hash = -(hash);
            return hash;
        };

        int compare(Hashable* elem) {
            return strcmp((char*)value, (char*)elem->value);
        };
};

#endif