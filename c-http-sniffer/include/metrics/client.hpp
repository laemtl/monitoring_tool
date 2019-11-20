#ifndef __CLIENTS_H__
#define __CLIENTS_H__

#include "hashTable.hpp"
#include "cf_list.h"
#include "metricCumDistr.hpp"
#include <inttypes.h>
#include <sys/types.h>
#include "analysis.hpp"


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
		Client(Analysis* analysis);
        void subscribe(EventManager* em);
        void cflAdd(Hashable* addr, int cnt);
		void cflAdd(int i, int cnt);
        void extract_freq_ht();

        void onNewFlowReceived(Flow* flow);
		void onFlowUpdate(Flow *flow);
		void onRequestReceived(pair_t *pair, Flow* flow);
		void onResponseReceived(pair_t *pair, Flow* flow);
		void onTimerExpired();
        void onIntervalExpired();
        void onAnalysisEnded();
};

#endif