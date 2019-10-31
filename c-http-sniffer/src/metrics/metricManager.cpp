#include "metricManager.hpp"

MetricManager::MetricManager() {
    metrics.push_back(new Rst());
    metrics.push_back(new ReqRate());
    metrics.push_back(new ErrRate());
    metrics.push_back(new Tp());
    metrics.push_back(new TpRev());

    metrics.push_back(new ConnRate());
}

void MetricManager::getMetrics() {
}