#ifndef __OBSERVER_H__
#define __OBSERVER_H__

namespace _protocol {
    class Pair;
}
class Flow;

class Observer {
    public:  
        virtual void onNewFlowReceived(Flow *flow) = 0;
        virtual void onFlowUpdate(Flow *flow) = 0;
        virtual void onRequestReceived(_protocol::Pair *pair, Flow *flow) = 0;
        virtual void onResponseReceived(_protocol::Pair *pair, Flow *flow) = 0;
        virtual void onTimerExpired() = 0;
        virtual void onIntervalExpired() = 0;
        virtual void onAnalysisEnded() = 0;
};

#endif