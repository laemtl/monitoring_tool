#ifndef __FLOW_UPDATE_H__
#define __FLOW_UPDATE_H__

#include "event.hpp"
#include "observer.hpp"
//#include "flow.hpp"

class Flow;
class FlowUpdate: public Event {
    public:
        void notify(Flow *flow);
};

#endif