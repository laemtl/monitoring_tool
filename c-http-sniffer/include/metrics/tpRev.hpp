#ifndef __TP_REV_H__
#define __TP_REV_H__

#include "tp.hpp"

class TpRev: public MetricAvg
{
    private:
      u_int32_t timerRef;
      u_int32_t intervalRef;
    public:
      TpRev(Analysis* analysis);
      void subscribe(EventManager* em);
      void onNewFlowReceived(flow_t *flow);
      void onFlowUpdate(flow_t *flow);
      void onRequestReceived(http_pair_t *pair, flow_t *flow);
      void onResponseReceived(http_pair_t *pair, flow_t *flow);
      void onTimerExpired();
      
      void reset();
      double getAvg();
};

#endif