#ifndef __VALUE_H__
#define __VALUE_H__

#include <pthread.h>
#include <type_traits>
#include <limits>
#include <math.h>

#include <iostream>
#include <string>
#include <stdio.h>

#define EPSILON	1e-7

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