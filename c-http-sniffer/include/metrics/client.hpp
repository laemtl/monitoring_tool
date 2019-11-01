#ifndef __CLIENTS_H__
#define __CLIENTS_H__

#include "hashTable.hpp"
#include "cf_list.h"
#include "metricCumDistr.hpp"
#include <inttypes.h>
#include <sys/types.h>

typedef Hashable Addr2;

class Client: public MetricCumDistr
{
	private:
		int req_tot;
        int flow_tot;
		Hash* ht;
        cf_list cfl;

	public:
		Client();
        void subscribe(EventManager* em);
        void cflAdd(Addr2* addr, int cnt);
        void add_client(u_int32_t saddr);
        void extract_freq_ht();

        void onNewFlowReceived(flow_t *flow);
		void onFlowUpdate(flow_t *flow);
		void onRequestReceived(http_pair_t *pair, flow_t *flow);
		void onResponseReceived(http_pair_t *pair, flow_t *flow);
		void onTimerExpired();
        void onIntervalExpired();
};

#endif