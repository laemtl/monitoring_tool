#ifndef __REQ_TYPE_H__
#define __REQ_TYPE_H__

#include <inttypes.h>
#include <sys/types.h>
#include <map> 
#include "metricCumDistr.hpp"

class Hashable;
class Analysis;
class EventManager;
class Flow;

class ReqType : public MetricCumDistr
{
	private:
		int reqTotal;
        std::map<char*, int> reqType;
		std::map<char*, int>::iterator it;
	public:
		ReqType(_protocol::Protocol* protocol, Analysis* analysis);
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

#include "cf_list.h"

#endif