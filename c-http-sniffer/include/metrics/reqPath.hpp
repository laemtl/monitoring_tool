#ifndef __REQ_PATH_H__
#define __REQ_PATH_H__

#include <inttypes.h>
#include <sys/types.h>
#include <libgen.h>

#include "metricCumDistr.hpp"

class ReqPath: public MetricCumDistr
{
	private:
		int reqTotal;
		Hash* ht;

	public:
		ReqPath(_protocol::Protocol* protocol, Analysis* analysis);
        void subscribe(EventManager* em);
        void cflAdd(Hashable* elem, int cnt);
		void cflAdd(int i, int cnt);

		char* extractReqPath(const char* uri);

        void onNewFlowReceived(Flow* flow);
		void onFlowUpdate(Flow* flow);
		void onRequestReceived(_protocol::Pair *pair, Flow* flow);
		void onResponseReceived(_protocol::Pair *pair, Flow* flow);
		void onTimerExpired();
        void onIntervalExpired();
		void onAnalysisEnded();
};

#include "analysis.hpp"
#include "hashTable.hpp"
#include "cf_list.h"
#include "stringHash.hpp"

#endif