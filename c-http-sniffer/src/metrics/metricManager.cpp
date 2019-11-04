#include "metricManager.hpp"
#include "rst.hpp"
#include "reqRate.hpp"
#include "errRate.hpp"
#include "tp.hpp"
#include "tpRev.hpp"
#include "connRate.hpp"
#include "client.hpp"
#include "reqPath.hpp"
#include "reqMethod.hpp"
#include "reqType.hpp"
#include "rspStatus.hpp"

MetricManager::MetricManager(Analysis* analysis) {
    metrics.push_back(new Rst(analysis));
    metrics.push_back(new ReqRate(analysis));
    metrics.push_back(new ErrRate(analysis));
    metrics.push_back(new Tp(analysis));
    metrics.push_back(new TpRev(analysis));
    metrics.push_back(new ConnRate(analysis));

    metrics.push_back(new Client(analysis));
    metrics.push_back(new ReqPath(analysis));
    metrics.push_back(new ReqMethod(analysis));
    metrics.push_back(new ReqType(analysis));
    metrics.push_back(new RspStatus(analysis));
}

void MetricManager::getMetrics() {
}