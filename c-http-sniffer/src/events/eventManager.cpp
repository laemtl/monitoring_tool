#include "eventManager.hpp"

EventManager::EventManager() {
    /*events.insert(pair<string, Event*>("newFlowReceived", new NewFlowReceived())); 
    events.insert(pair<string, Event*>("flowUpdate", new FlowUpdate())); 
    events.insert(pair<string, Event*>("requestReceived", new RequestReceived()));
    events.insert(pair<string, Event*>("responseReceived", new ResponseReceived())); 
    events.insert(pair<string, Event*>("timerExpired", new TimerExpired()));
    events.insert(pair<string, Event*>("intervalExpired", new IntervalExpired()));*/

    newFlowReceived = new NewFlowReceived(); 
    flowUpdate = new FlowUpdate(); 
    requestReceived = new RequestReceived();
    responseReceived = new ResponseReceived(); 
    timerExpired = new TimerExpired();
    intervalExpired = new IntervalExpired();
}

/*Event* EventManager::get(string e) {
    for (auto it = events.find(e); it != events.end(); it++) 
        return it->second;
}*/