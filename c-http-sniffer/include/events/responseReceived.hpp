#ifndef __RESPONSE_RECEIVED_H__
#define __RESPONSE_RECEIVED_H__

#include "event.hpp"
#include "observer.hpp"
//#include "flow.hpp"

class Flow;
class ResponseReceived: public Event {
    public:
        void notify(Pair *pair, Flow *flow);
};

#endif