#ifndef __OBSERVER_H__
#define __OBSERVER_H__
 
#include "flow.h"

class Observer
{
    public:   
        virtual void onNewFlowReceived(flow_t *flow) = 0;
        virtual void onFlowUpdate(flow_t *flow) = 0;
        virtual void onRequestReceived(http_pair_t *pair, flow_t *flow) = 0;
        virtual void onResponseReceived(http_pair_t *pair, flow_t *flow) = 0;
        virtual void onTimerExpired() = 0;
        virtual void onIntervalExpired() = 0;
};

#endif