#ifndef __EVENT_H__
#define __EVENT_H__

#include <vector>
#include <algorithm>

class Observer;

class Event {
    protected:
        std::vector<Observer*> observers;
    public:
        void add(Observer* obs);
        void remove(Observer* obs);
};

#include "observer.hpp"

#endif