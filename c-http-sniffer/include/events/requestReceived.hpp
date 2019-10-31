#ifndef __REQUEST_RECEIVED_H__
#define __REQUEST_RECEIVED_H__

#include "event.hpp"
#include "observer.hpp"
#include "flow.h"

class RequestReceived: public Event {
    public:
        void notify(http_pair_t *pair, flow_t *flow);
};

#endif