#ifndef __EVENT_MANAGER_H__
#define __EVENT_MANAGER_H__

#include "newFlowReceived.hpp"
#include "flowUpdate.hpp"
#include "requestReceived.hpp"
#include "responseReceived.hpp"
#include "timerExpired.hpp"
#include "intervalExpired.hpp"

class EventManager 
{
    public:
        EventManager();

        NewFlowReceived* newFlowReceived;
        FlowUpdate* flowUpdate;
        RequestReceived* requestReceived;
        ResponseReceived* responseReceived;
        TimerExpired* timerExpired;
        IntervalExpired* intervalExpired;
};

#endif