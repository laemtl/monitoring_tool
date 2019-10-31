#ifndef __FLOW_UPDATE_H__
#define __FLOW_UPDATE_H__

#include "event.hpp"
#include "observer.hpp"
#include "flow.h"

class FlowUpdate: public Event {
    public:
        void notify(flow_t *flow);
};

#endif