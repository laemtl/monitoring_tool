#include "event.hpp"
#include "observer.hpp"

class TimerExpired: public Event {
    public:
        void notify();
};

void TimerExpired::notify() {
    for (Observer *observer : observers) {
        observer.onTimerExpired();
    }
}