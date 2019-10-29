#ifndef __EVENT_H__
#define __EVENT_H__

#include <vector>
#include <algorithm>
#include "observer.hpp"

using namespace std;

class Event
{
    protected:
        vector<Observer*> observers;
    public:
        Event();
        void add(Observer* obs);
        void remove(Observer* obs);
};

#endif