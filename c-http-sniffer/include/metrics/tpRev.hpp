#ifndef __TP_REV_H__
#define __TP_REV_H__

#include "metricAvg.hpp"

class TpRev: public MetricAvg
{
    private:
      u_int32_t timerRef;
      u_int32_t intervalRef;
    public:
      TpRev(_protocol::Protocol* protocol, Analysis* analysis);
      void subscribe(EventManager* em);
      void onNewFlowReceived(Flow* flow);
      void onFlowUpdate(Flow* flow);
      void onRequestReceived(_protocol::Pair *pair, Flow* flow);
      void onResponseReceived(_protocol::Pair *pair, Flow* flow);
      void onTimerExpired();
      
      void reset();
      double getAvg();
};

#include "tp.hpp"

#endif