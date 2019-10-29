#ifndef __RESPONSE_RECEIVED_H__
#define __RESPONSE_RECEIVED_H__

#include "event.hpp"
#include "flow.h"

class ResponseReceived: public Event {
    public:
        void notify(http_pair_t *pair, flow_t *flow);
};

#endif