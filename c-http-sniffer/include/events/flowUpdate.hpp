#ifndef __FLOW_UPDATE_H__
#define __FLOW_UPDATE_H__

#include "event.hpp"
class Flow;

class FlowUpdate: public Event {
    public:
        void notify(Flow *flow);
};

#include "observer.hpp"
#include "flow.hpp"

#endif