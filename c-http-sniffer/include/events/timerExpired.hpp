#ifndef __TIMER_EXPIRED_H__
#define __TIMER_EXPIRED_H__

#include "event.hpp"
#include "observer.hpp"

class TimerExpired: public Event {
    public:
        void notify();
};

#endif