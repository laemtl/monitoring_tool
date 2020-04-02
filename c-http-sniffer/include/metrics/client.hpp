#ifndef __CLIENTS_H__
#define __CLIENTS_H__

#include <inttypes.h>
#include <sys/types.h>

#include "metricCumDistr.hpp"

class Flow;
class Analysis;
namespace _protocol {
	class Protocol;
}

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
		Client(_protocol::Protocol* protocol, Analysis* analysis);
        void subscribe(EventManager* em);
        void cflAdd(Hashable* addr, int cnt);
		void cflAdd(int i, int cnt);
        void extract_freq_ht();

        void onNewFlowReceived(Flow* flow);
		void onFlowUpdate(Flow *flow);
		void onRequestReceived(_protocol::Pair *pair, Flow* flow);
		void onResponseReceived(_protocol::Pair *pair, Flow* flow);
		void onTimerExpired();
        void onIntervalExpired();
        void onAnalysisEnded();
};

#include "analysis.hpp"
#include "flow.hpp"
#include "cf_list.h"
#include "hashTable.hpp"

#endif