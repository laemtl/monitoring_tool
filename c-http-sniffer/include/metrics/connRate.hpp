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
		ConnRate(Analysis* analysis);
		void subscribe(EventManager* em);
    	void onNewFlowReceived(Flow* flow);
		void onFlowUpdate(Flow* flow);
		void onRequestReceived(pair_t *pair, Flow* flow);
		void onResponseReceived(pair_t *pair, Flow* flow);
		void onTimerExpired();

		double getAvg();
};

#endif