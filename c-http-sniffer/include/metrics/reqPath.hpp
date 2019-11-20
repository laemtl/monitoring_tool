#ifndef __REQ_PATH_H__
#define __REQ_PATH_H__

#include "analysis.hpp"
#include "hashTable.hpp"
#include "cf_list.h"
#include "metricCumDistr.hpp"
#include "stringHash.hpp"
#include <inttypes.h>
#include <sys/types.h>
#include <libgen.h>

class ReqPath: public MetricCumDistr
{
	private:
		int reqTotal;
        //int flow_tot;
		Hash* ht;

	public:
		ReqPath(Analysis* analysis);
        void subscribe(EventManager* em);
        void cflAdd(Hashable* elem, int cnt);
		void cflAdd(int i, int cnt);

		char* extractReqPath(const char* uri);

        void onNewFlowReceived(Flow* flow);
		void onFlowUpdate(Flow* flow);
		void onRequestReceived(pair_t *pair, Flow* flow);
		void onResponseReceived(pair_t *pair, Flow* flow);
		void onTimerExpired();
        void onIntervalExpired();
		void onAnalysisEnded();
};

#endif