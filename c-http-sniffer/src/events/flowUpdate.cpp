#include "flowUpdate.hpp"

void FlowUpdate::notify(Flow* flow) {
    for (Observer *observer : observers) {
        observer->onFlowUpdate(flow);
    }
}