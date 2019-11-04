#ifndef __REQ_METHOD_H__
#define __REQ_METHOD_H__

#include "hashTable.hpp"
#include "cf_list.h"
#include "metricCumDistr.hpp"
#include "stringHash.hpp"
#include <inttypes.h>
#include <sys/types.h>
#include <libgen.h>

class ReqMethod: public MetricCumDistr
{
	private:
		int reqTotal;
		Hash* ht;

	public:
		ReqMethod(Analysis* analysis);
        void subscribe(EventManager* em);
        void cflAdd(Hashable* elem, int cnt);
		void cflAdd(int i, int cnt);
		char* extractReqMethod(const char* uri);

        void onNewFlowReceived(flow_t *flow);
		void onFlowUpdate(flow_t *flow);
		void onRequestReceived(http_pair_t *pair, flow_t *flow);
		void onResponseReceived(http_pair_t *pair, flow_t *flow);
		void onTimerExpired();
        void onIntervalExpired();
};

#endif