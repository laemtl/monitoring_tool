#ifndef __OBSERVER_H__
#define __OBSERVER_H__
 
#include "flow.hpp"

class Observer
{
    private:  
        
    public:   
        Observer();
        void onNewFlowReceived(Flow *flow);
        void onFlowUpdate(Flow *flow);
        void onRequestReceived(http_pair_t *pair, Flow *flow);
        void onResponseReceived(http_pair_t *pair, Flow *flow);
        void onTimerExpired();
        void onIntervalExpired();
};

#endif