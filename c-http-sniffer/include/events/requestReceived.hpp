#ifndef __REQUEST_RECEIVED_H__
#define __REQUEST_RECEIVED_H__

#include "event.hpp"
#include "observer.hpp"
#include "protocol.hpp"
//#include "flow.hpp"

class Flow;
class RequestReceived: public Event {
    public:
        void notify(Pair *pair, Flow *flow);
};

#endif