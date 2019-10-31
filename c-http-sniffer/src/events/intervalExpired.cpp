#include "intervalExpired.hpp"

void IntervalExpired::notify() {
    for (Observer *observer : observers) {
        observer->onIntervalExpired();
    }
}