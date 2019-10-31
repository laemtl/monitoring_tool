#ifndef __ERR_RATE_H__
#define __ERR_RATE_H__

#include "metricAvg.hpp"

class ErrRate: public MetricAvg
{
  public:
    ErrRate();
    void subscribe(EventManager* em);
    void onNewFlowReceived(flow_t *flow);
    void onFlowUpdate(flow_t *flow);
    void onRequestReceived(http_pair_t *pair, flow_t *flow);
    void onResponseReceived(http_pair_t *pair, flow_t *flow);
    void onTimerExpired();

    double getRate();
};

#endif