#ifndef __ANALYSIS_ENDED_H__
#define __ANALYSIS_ENDED_H__

#include "event.hpp"

class AnalysisEnded: public Event {
    public:
        void notify();
};

#include "observer.hpp"

#endif