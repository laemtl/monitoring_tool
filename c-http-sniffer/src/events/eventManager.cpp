#include "eventManager.hpp"

EventManager::EventManager() {
    newFlowReceived = new NewFlowReceived(); 
    flowUpdate = new FlowUpdate(); 
    requestReceived = new RequestReceived();
    responseReceived = new ResponseReceived(); 
    timerExpired = new TimerExpired();
    intervalExpired = new IntervalExpired();
}