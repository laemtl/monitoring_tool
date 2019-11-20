#ifndef __REQ_TYPE_H__
#define __REQ_TYPE_H__

#include "hashTable.hpp"
#include "cf_list.h"
#include "metricCumDistr.hpp"
#include <inttypes.h>
#include <sys/types.h>
//#include <vector>

class ReqType : public MetricCumDistr
{
	private:
		int reqTotal;
        //int flow_tot;
        int reqType[41] = {0};
		int reqTypeSize;

	public:
		ReqType(Analysis* analysis);
        void subscribe(EventManager* em);

        void cflAdd(Hashable* elem, int cnt);
		void cflAdd(int index, int cnt);
        void extract_freq_ht();

        void onNewFlowReceived(Flow* flow);
		void onFlowUpdate(Flow* flow);
		void onRequestReceived(pair_t *pair, Flow* flow);
		void onResponseReceived(pair_t *pair, Flow* flow);
		void onTimerExpired();
        void onIntervalExpired();
		void onAnalysisEnded();
};

#endif