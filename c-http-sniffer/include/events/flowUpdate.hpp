#ifndef __FLOW_UPDATE_H__
#define __FLOW_UPDATE_H__

#include "event.hpp"
#include "flow.hpp"

class FlowUpdate: public Event {
    public:
        void notify(Flow *flow);
};

#endif