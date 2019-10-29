#ifndef __RST_H__
#define __RST_H__

#include "metricAvg.hpp"
#include "observer.hpp"
#include <inttypes.h>

class Rst: public MetricAvg
{
    public:
		void subscribe(EventManager* em);
		
		virtual void onNewFlowReceived(flow_t *flow);
        virtual void onFlowUpdate(flow_t *flow);
        virtual void onRequestReceived(http_pair_t *pair, flow_t *flow);
        virtual void onResponseReceived(http_pair_t *pair, flow_t *flow);
        virtual void onTimerExpired();
        virtual void onIntervalExpired();
};

#endif