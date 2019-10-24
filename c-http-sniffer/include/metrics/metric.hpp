#ifndef __METRIC_H__
#define __METRIC_H__
 
#include "value.hpp"
#include "observer.hpp"
#include "analysis.hpp"

template <class Num>
class Metric: public Observer
{
    private:  
        bool active;
        Analysis *analysis;
        Value<Num> subtotal;
        Value<Num> total;
        
        Value<Num> sum;
        Value<Num> min;
        Value<Num> max;

    public:   
        Metric();   
        bool isActive();
        void setActive(bool a);
        void reset();
        Num getSubtotal();
        Num getTotal();
        void setTotal(Num n);
        Num getSum();
        Num getMax();
        Num getMin();
        void addSum(Num n);
        void addTotal(Num n);
        void addSubtotal(Num n);
        void resetSubtotal();
        void setMin(Num n);
        void setMax(Num n);
};

template <class Num>
bool Metric<Num>::isActive() {
    return active;
}

template <class Num>
void Metric<Num>::setActive( bool a) {
    active = a;
}

/*void _Metric::onTimerExpires() {
	// reset interval (1 sec) counter
	reset_int_cnt(&ht);
}*/

template <class Num>
Metric<Num>::Metric() {
    subtotal = new Value<Num>();
    total = new Value<Num>();
        
    sum = new Value<Num>();
    min = new Value<Num>();
    max = new Value<Num>();

    reset();
}   

template <class Num>
void Metric<Num>::reset() {
	total.setVal(0);
	subtotal.setVal(0);
	sum.setVal(0);
	min.setVal(numeric_limits<Num>::max());
	max.setVal(0);
}

template <class Num>
Num Metric<Num>::getSubtotal() {
    return subtotal.getVal();
}

template <class Num>
Num Metric<Num>::getTotal() {
    return total.getVal();
}

template <class Num>
Num Metric<Num>::getSum() {
    return sum.getVal();
}

template <class Num>
Num Metric<Num>::getMin() {
	return min.getVal();
}

template <class Num>
Num Metric<Num>::getMax() {
	return max.getVal();
}

template <class Num>
void Metric<Num>::addSum(Num n) {
	sum.addVal(n);
}

template <class Num>
void Metric<Num>::addTotal(Num n) {
    total.addVal(n);
}

template <class Num>
void Metric<Num>::setTotal(Num n) {
    total.setVal(n);
}

template <class Num>
void Metric<Num>::addSubtotal(Num n) {
    subtotal.addVal(n);
}

template <class Num>
void Metric<Num>::resetSubtotal() {
	subtotal.setVal(0);
}

template <class Num>
void Metric<Num>::setMin(Num n) {
    min.minimize(n);
}

template <class Num>
void Metric<Num>::setMax(Num n) {
    max.maximize(n);
}

/*template <class Num>
void Metric<Num>::onTimerExpires() {
	// reset interval (1 sec) counter
	reset_int_cnt(&ht);
}*/

#endif