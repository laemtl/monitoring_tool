#include "metric.hpp"

bool Metric2::isActive() {
    return active;
}

void Metric2::setActive( bool a) {
    active = a;
}

/*void _Metric::onTimerExpires() {
	// reset interval (1 sec) counter
	reset_int_cnt(&ht);
}*/

Metric2::Metric2() {
    subtotal = new Value();
    total = new Value();
    sum = new Value();
    min = new Value();
    max = new Value();

    reset();
}   

void Metric2::subscribe(EventManager* em) {
}

void Metric2::reset() {
	total->set(0);
	subtotal->set(0);
	sum->set(0);
	min->set(numeric_limits<double>::max());
	max->set(0);
}

/*double Metric2::getSubtotal() {
    return subtotal->get();
}

double Metric2::getTotal() {
    return total->get();
}

double Metric2::getSum() {
    return sum->get();
}

double Metric2::getMin() {
	return min->get();
}

double Metric2::getMax() {
	return max->get();
}

void Metric2::addSum(double n) {
	sum->add(n);
}

void Metric2::addTotal(double n) {
    total->add(n);
}

void Metric2::setTotal(double n) {
    total->set(n);
}

void Metric2::addSubtotal(double n) {
    subtotal->add(n);
}*/

void Metric2::resetSubtotal() {
	subtotal->set(0);
}

void Metric2::setMin(double n) {
    min->minimize(n);
}

void Metric2::setMax(double n) {
    max->maximize(n);
}

/*void Metric<double>::onTimerExpires() {
	// reset interval (1 sec) counter
	reset_int_cnt(&ht);
}*/
