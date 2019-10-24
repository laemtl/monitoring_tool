#ifndef __REQUEST_RECEIVED_H__
#define __REQUEST_RECEIVED_H__

#include "event.hpp"
#include "flow.hpp"

class RequestReceived: public Event {
    public:
        void notify(http_pair_t *pair, Flow *flow);
};

#endif