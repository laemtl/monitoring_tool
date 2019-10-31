#include "flowUpdate.hpp"

void FlowUpdate::notify(flow_t *flow) {
    for (Observer *observer : observers) {
        observer->onFlowUpdate(flow);
    }
}