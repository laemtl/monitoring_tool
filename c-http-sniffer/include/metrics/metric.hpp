#ifndef __METRIC_H__
#define __METRIC_H__
#include "eventManager.hpp"

class Metric2 {
    public:  
        virtual void subscribe(EventManager* em) = 0;
};

#endif