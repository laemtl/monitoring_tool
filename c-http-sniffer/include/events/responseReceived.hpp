#ifndef __RESPONSE_RECEIVED_H__
#define __RESPONSE_RECEIVED_H__

#include "event.hpp"
class Flow;

class ResponseReceived: public Event {
    public:
        void notify(_protocol::Pair *pair, Flow *flow);
};

#include "observer.hpp"
#include "flow.hpp"

#endif