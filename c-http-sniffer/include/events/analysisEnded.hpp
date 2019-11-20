#ifndef __ANALYSIS_ENDED_H__
#define __ANALYSIS_ENDED_H__

#include "event.hpp"
#include "observer.hpp"

class AnalysisEnded: public Event {
    public:
        void notify();
};

#endif