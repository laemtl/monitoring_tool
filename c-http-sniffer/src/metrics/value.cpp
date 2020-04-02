#include "value.hpp"

Value::Value() {
    pthread_mutex_init(&(mutex), NULL);
    set(0);
}

double Value::get() {
    if(isClose(std::numeric_limits<double>::max())) {
        return 0;
    }
    
    double v;
    pthread_mutex_lock(&(mutex));
    v = val;
    pthread_mutex_unlock(&(mutex));

    return val;
}

void Value::set(double v) {
    pthread_mutex_lock(&(mutex));
    val = v;
    pthread_mutex_unlock(&(mutex));
}

void Value::add(double v) {
    pthread_mutex_lock(&(mutex));
    val += v;
    pthread_mutex_unlock(&(mutex));
}

void Value::minimize(double v) {
    pthread_mutex_lock(&(mutex));
    if(v < val) val = v;
    pthread_mutex_unlock(&(mutex)); 
}

void Value::maximize(double v) {
    pthread_mutex_lock(&(mutex));
    if(v > val) val = v;
    pthread_mutex_unlock(&(mutex)); 
}

bool Value::isClose(double v) {
    return ( fabs(val - v) < EPSILON );
}