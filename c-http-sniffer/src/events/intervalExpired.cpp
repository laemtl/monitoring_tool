#include "event.hpp"
#include "observer.hpp"

class IntervalExpired: public Event {
    public:
        void notify();
};

void IntervalExpired::notify() {
    for (Observer *observer : observers) {
        observer.onIntervalExpired();
    }
}