#ifndef __REQUEST_RECEIVED_H__
#define __REQUEST_RECEIVED_H__

#include "event.hpp"
#include "observer.hpp"
#include "flow.hpp"

class RequestReceived: public Event {
    public:
        void notify(pair_t *pair, Flow *flow);
};

#endif