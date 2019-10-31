#ifndef __TP_REV_H__
#define __TP_REV_H__

#include "tp.hpp"

class TpRev: public Tp
{
    public:
      TpRev();
      void onFlowUpdate(flow_t *flow);
};

#endif