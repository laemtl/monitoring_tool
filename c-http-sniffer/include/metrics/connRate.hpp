#ifndef __CONN_RATE_H__
#define __CONN_RATE_H__

#include "metricAvg.hpp"

#include <sys/types.h>
#include "util.h"
#include "addr.hpp"
#include "analysis.hpp"

// Get connection count per interval
class ConnRate: public MetricAvg
{
	public:
		ConnRate(Protocol* protocol, Analysis* analysis);
		void subscribe(EventManager* em);
    	void onNewFlowReceived(Flow* flow);
		void onFlowUpdate(Flow* flow);
		void onRequestReceived(Pair *pair, Flow* flow);
		void onResponseReceived(Pair *pair, Flow* flow);
		void onTimerExpired();

		double getAvg();
};

#endif