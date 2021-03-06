#ifndef __REQ_METHOD_H__
#define __REQ_METHOD_H__

#include <inttypes.h>
#include <sys/types.h>
#include <libgen.h>
#include "metricCumDistr.hpp"

class ReqMethod: public MetricCumDistr
{
	private:
		int reqTotal;
		Hash* ht;

	public:
		ReqMethod(_protocol::Protocol* protocol, Analysis* analysis);
        void subscribe(EventManager* em);
        void cflAdd(Hashable* elem, int cnt);
		void cflAdd(int i, int cnt);
		char* extractReqMethod(const char* uri);

        void onNewFlowReceived(Flow* flow);
		void onFlowUpdate(Flow* flow);
		void onRequestReceived(_protocol::Pair *pair, Flow* flow);
		void onResponseReceived(_protocol::Pair *pair, Flow* flow);
		void onTimerExpired();
        void onIntervalExpired();
		void onAnalysisEnded();
};

#include "hashTable.hpp"
#include "cf_list.h"
#include "stringHash.hpp"

#endif