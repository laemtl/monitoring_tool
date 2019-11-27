#ifndef __TP_REV_H__
#define __TP_REV_H__

#include "tp.hpp"

class TpRev: public MetricAvg
{
    private:
      u_int32_t timerRef;
      u_int32_t intervalRef;
    public:
      TpRev(Protocol* protocol, Analysis* analysis);
      void subscribe(EventManager* em);
      void onNewFlowReceived(Flow* flow);
      void onFlowUpdate(Flow* flow);
      void onRequestReceived(Pair *pair, Flow* flow);
      void onResponseReceived(Pair *pair, Flow* flow);
      void onTimerExpired();
      
      void reset();
      double getAvg();
};

#endif