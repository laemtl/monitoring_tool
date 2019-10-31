#ifndef __CONN_RATE_H__
#define __CONN_RATE_H__

#include "attr.h"

#include "metricAvg.hpp"

#include <sys/types.h>
#include "hash_table.h"
#include "util.h"
#include "addr.hpp"

u_int32_t conn_hash_fn(Attr* c);
int addr_compare(Attr* c1, Attr* c2);
BOOL is_conn_ht(hash_t* ht);


// Get connection count per interval
class ConnRate: public MetricAvg
{
	public:
		ConnRate();
		void subscribe(EventManager* em);
    	void onNewFlowReceived(flow_t *flow);
		void onFlowUpdate(flow_t *flow);
		void onRequestReceived(http_pair_t *pair, flow_t *flow);
		void onResponseReceived(http_pair_t *pair, flow_t *flow);
		void onTimerExpired();
};

#endif