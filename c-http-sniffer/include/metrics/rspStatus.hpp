#ifndef __RSP_STATUS_H__
#define __RSP_STATUS_H__

#include <inttypes.h>
#include <sys/types.h>
#include "metricCumDistr.hpp"

class RspStatus : public MetricCumDistr
{
	private:
		int rspTotal;
        std::map<char*, int> rspStatus;
		std::map<char*, int>::iterator it;
	public:
		RspStatus(_protocol::Protocol* protocol, Analysis* analysis);
        void subscribe(EventManager* em);

        void cflAdd(Hashable* elem, int cnt);
		void extract_freq_ht();

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

#endif