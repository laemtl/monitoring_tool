#ifndef __REQUEST_RECEIVED_H__
#define __REQUEST_RECEIVED_H__

#include "event.hpp"
class Flow;

class RequestReceived: public Event {
    public:
        void notify(_protocol::Pair *pair, Flow *flow);
};

#include "observer.hpp"
#include "protocol.hpp"
#include "flow.hpp"

#endif