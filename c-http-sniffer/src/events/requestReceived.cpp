#include "requestReceived.hpp"

void RequestReceived::notify(_protocol::Pair *pair, Flow* flow) {
    for (Observer *observer : observers) {
        observer->onRequestReceived(pair, flow);
    }
}