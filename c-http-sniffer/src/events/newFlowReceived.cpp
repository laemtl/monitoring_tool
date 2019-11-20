#include "newFlowReceived.hpp"

void NewFlowReceived::notify(Flow* flow) {
    for (Observer *observer : observers) {
        observer->onNewFlowReceived(flow);
    }
}