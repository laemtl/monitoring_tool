#include "newFlowReceived.hpp"

void NewFlowReceived::notify(flow_t *flow) {
    for (Observer *observer : observers) {
        observer->onNewFlowReceived(flow);
    }
}