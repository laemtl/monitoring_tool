#include "timerExpired.hpp"

void TimerExpired::notify() {
    for (Observer *observer : observers) {
        observer->onTimerExpired();
    }
}