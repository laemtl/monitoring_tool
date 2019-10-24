#ifndef __INTERVAL_EXPIRED_H__
#define __INTERVAL_EXPIRED_H__

#include "event.hpp"

class IntervalExpired: public Event {
    public:
        void notify();
};

#endif