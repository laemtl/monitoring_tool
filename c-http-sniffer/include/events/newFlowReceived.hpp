#ifndef __NEW_FLOW_RECEIVED_H__
#define __NEW_FLOW_RECEIVED_H__

#include "event.hpp"
#include "flow.hpp"

class NewFlowReceived: public Event {
    public:
        void notify(Flow *flow);
};

#endif