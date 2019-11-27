#ifndef __EVENT_H__
#define __EVENT_H__

#include <vector>
#include <algorithm>

using namespace std;

class Observer;
class Event {
    protected:
        vector<Observer*> observers;
    public:
        void add(Observer* obs);
        void remove(Observer* obs);
};

#endif