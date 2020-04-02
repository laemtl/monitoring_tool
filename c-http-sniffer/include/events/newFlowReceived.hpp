#ifndef __NEW_FLOW_RECEIVED_H__
#define __NEW_FLOW_RECEIVED_H__

#include "event.hpp"
class Flow;

class NewFlowReceived: public Event {
    public:
        void notify(Flow *flow);
};

#include "observer.hpp"
#include "flow.hpp"

#endif