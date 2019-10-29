#ifndef __EVENT_MANAGER_H__
#define __EVENT_MANAGER_H__

// or dimplify with functions in analysis
#include <map>
#include "event.hpp"
#include "newFlowReceived.hpp"
#include "flowUpdate.hpp"
#include "requestReceived.hpp"
#include "responseReceived.hpp"
#include "timerExpired.hpp"
#include "intervalExpired.hpp"

class EventManager 
{
    private:
        //map<string, Event*> events;
    public:
        EventManager();

        NewFlowReceived* newFlowReceived;
        FlowUpdate* flowUpdate;
        RequestReceived* requestReceived;
        ResponseReceived* responseReceived;
        TimerExpired* timerExpired;
        IntervalExpired* intervalExpired;
        
        //Event* get(string e);
};

#endif