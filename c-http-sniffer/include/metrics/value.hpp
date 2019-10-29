#ifndef __VALUE_H__
#define __VALUE_H__

#include <pthread.h>
#include <type_traits>
#include <limits>
#include <math.h>

#define EPSILON	1e-7

using namespace std; 

class Value {
    double val;
    pthread_mutex_t mutex;

    public:
        double get();
        void set(double v);
        void add(double v);
        void minimize(double v);
        void maximize(double v);
        bool isClose(double v);

        Value();
};

#endif