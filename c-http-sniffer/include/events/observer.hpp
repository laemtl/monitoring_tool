#ifndef __OBSERVER_H__
#define __OBSERVER_H__
 
#include "flow.hpp"
        
class Observer
{
    public:  
        virtual void onNewFlowReceived(Flow *flow) = 0;
        virtual void onFlowUpdate(Flow *flow) = 0;
        virtual void onRequestReceived(pair_t *pair, Flow *flow) = 0;
        virtual void onResponseReceived(pair_t *pair, Flow *flow) = 0;
        virtual void onTimerExpired() = 0;
        virtual void onIntervalExpired() = 0;
        virtual void onAnalysisEnded() = 0;
};

#endif