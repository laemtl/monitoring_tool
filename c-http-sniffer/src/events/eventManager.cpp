#include "eventManager.hpp"

EventManager::EventManager() {
    NewFlowReceived = new newFlowReceived();
    FlowUpdate = new flowUpdate();
    RequestReceived = new requestReceived();
    TimerExpired = new timerExpired();
    IntervalExpired = new intervalExpired();
}