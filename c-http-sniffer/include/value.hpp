#include <pthread.h>
#include <type_traits>
#include <limits>

#define EPSILON	1e-7

using namespace std; 


// todo: atomic instead lock

//std::atomic<int>

//template<class T,
//    typename std::enable_if<std::is_arithmetic<T>::value, T>::type
//> struct Num{};
//template <typename Num, typename std::enable_if<std::is_arithmetic<Num>::value>::type* = nullptr>
template <class Num>
class Value {
    Num val;
    pthread_mutex_t mutex;

    public:
        Num getVal();
        void setVal(Num v);
        void addVal(Num v);
        void minimize(Num v);
        void maximize(Num v);

        bool isClose(Num v);

        Value();
};

template <class Num>
Value<Num>::Value() {
    pthread_mutex_init(&(mutex), NULL);
}

template <class Num>
Num Value<Num>::getVal() {
    if(isClose(numeric_limits<Num>::max())) {
        return 0;
    }
    return val;
}

template <class Num>
void Value<Num>::setVal(Num v) {
    pthread_mutex_lock(&(mutex));
    val = v;
    pthread_mutex_unlock(&(mutex));
}

template <class Num>
void Value<Num>::addVal(Num v) {
    pthread_mutex_lock(&(mutex));
    val += v;
    pthread_mutex_unlock(&(mutex));
}

template <class Num>
void Value<Num>::minimize(Num v) {
    pthread_mutex_lock(&(mutex));
    if(v < val) val = v;
    pthread_mutex_unlock(&(mutex)); 
}

template <class Num>
void Value<Num>::maximize(Num v) {
    pthread_mutex_lock(&(mutex));
    if(v > val) val = v;
    pthread_mutex_unlock(&(mutex)); 
}

template <class Num>
bool Value<Num>::isClose(Num v) {
    return ( fabs(val - v) < EPSILON );
}