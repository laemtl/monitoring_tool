#ifndef __RESPONSE_RECEIVED_H__
#define __RESPONSE_RECEIVED_H__

#include "event.hpp"
#include "flow.hpp"

class ResponseReceived: public Event {
    public:
        void notify(http_pair_t *pair, Flow *flow);
};

#endif