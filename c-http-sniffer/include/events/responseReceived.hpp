#ifndef __RESPONSE_RECEIVED_H__
#define __RESPONSE_RECEIVED_H__

#include "event.hpp"
#include "observer.hpp"
#include "flow.hpp"

class ResponseReceived: public Event {
    public:
        void notify(pair_t *pair, Flow *flow);
};

#endif