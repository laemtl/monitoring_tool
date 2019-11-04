#ifndef __RSP_STATUS_H__
#define __RSP_STATUS_H__

#include "hashTable.hpp"
#include "cf_list.h"
#include "metricCumDistr.hpp"
#include <inttypes.h>
#include <sys/types.h>

class RspStatus : public MetricCumDistr
{
	private:
		int rspTotal;
        //int flow_tot;
        int rspStatus[599] = {0};
        int rspStatusSize;

	public:
		RspStatus(Analysis* analysis);
        void subscribe(EventManager* em);

        void cflAdd(Hashable* elem, int cnt);
		void cflAdd(int index, int cnt);
        void extract_freq_ht();

        void onNewFlowReceived(flow_t *flow);
		void onFlowUpdate(flow_t *flow);
		void onRequestReceived(http_pair_t *pair, flow_t *flow);
		void onResponseReceived(http_pair_t *pair, flow_t *flow);
		void onTimerExpired();
        void onIntervalExpired();
};

#endif