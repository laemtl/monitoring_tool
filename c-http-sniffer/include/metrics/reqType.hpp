#ifndef __REQ_TYPE_H__
#define __REQ_TYPE_H__

#include "cf_list.h"
#include "metricCumDistr.hpp"
#include <inttypes.h>
#include <sys/types.h>
//#include <vector>

class Hashable;
class Analysis;
class EventManager;
class Flow;

class ReqType : public MetricCumDistr
{
	private:
		int reqTotal;
        //int flow_tot;
        int* reqType;
		int reqTypeSize;

	public:
		ReqType(Protocol* protocol, Analysis* analysis);
        void subscribe(EventManager* em);

        void cflAdd(Hashable* elem, int cnt);
		void cflAdd(int index, int cnt);
        void extract_freq_ht();

        void onNewFlowReceived(Flow* flow);
		void onFlowUpdate(Flow* flow);
		void onRequestReceived(Pair *pair, Flow* flow);
		void onResponseReceived(Pair *pair, Flow* flow);
		void onTimerExpired();
        void onIntervalExpired();
		void onAnalysisEnded();
};

#include "hashTable.hpp"
#endif