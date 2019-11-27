#ifndef __CLIENTS_H__
#define __CLIENTS_H__

#include "cf_list.h"
#include <inttypes.h>
#include <sys/types.h>
#include "hashTable.hpp"
#include "metricCumDistr.hpp"

class Flow;
class Analysis;

class Addr2 : public Hashable
{
    public:
        Addr2(u_int32_t* val);
        u_int32_t hash();
        int compare(Hashable* elem);
};

class Client : public MetricCumDistr
{
	private:
		int reqTotal;
        //int flow_tot;
		Hash* ht;

	public:
		Client(Protocol* protocol, Analysis* analysis);
        void subscribe(EventManager* em);
        void cflAdd(Hashable* addr, int cnt);
		void cflAdd(int i, int cnt);
        void extract_freq_ht();

        void onNewFlowReceived(Flow* flow);
		void onFlowUpdate(Flow *flow);
		void onRequestReceived(Pair *pair, Flow* flow);
		void onResponseReceived(Pair *pair, Flow* flow);
		void onTimerExpired();
        void onIntervalExpired();
        void onAnalysisEnded();
};

#include "analysis.hpp"
#include "flow.hpp"

#endif