#ifndef __EVENT_MANAGER_H__
#define __EVENT_MANAGER_H__

class NewFlowReceived;
class FlowUpdate;
class RequestReceived;
class ResponseReceived;
class TimerExpired;
class IntervalExpired;
class AnalysisEnded;

class EventManager {
    public:
        EventManager();

        NewFlowReceived* newFlowReceived;
        FlowUpdate* flowUpdate;
        RequestReceived* requestReceived;
        ResponseReceived* responseReceived;
        TimerExpired* timerExpired;
        IntervalExpired* intervalExpired;
        AnalysisEnded* analysisEnded;
};

#include "newFlowReceived.hpp"
#include "flowUpdate.hpp"
#include "requestReceived.hpp"
#include "responseReceived.hpp"
#include "timerExpired.hpp"
#include "intervalExpired.hpp"
#include "analysisEnded.hpp"

#endif