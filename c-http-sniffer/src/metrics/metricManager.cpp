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

MetricManager::MetricManager(Protocol* protocol, Analysis* analysis) {
    metrics.push_back(new Rst(protocol, analysis));
    metrics.push_back(new ReqRate(protocol, analysis));
    metrics.push_back(new ErrRate(protocol, analysis));
    metrics.push_back(new Tp(protocol, analysis));
    metrics.push_back(new TpRev(protocol, analysis));
    metrics.push_back(new ConnRate(protocol, analysis));

    metrics.push_back(new Client(protocol, analysis));
    //metrics.push_back(new ReqPath(protocol, analysis));
    //metrics.push_back(new ReqMethod(protocol, analysis));
    metrics.push_back(new ReqType(protocol, analysis));
    metrics.push_back(new RspStatus(protocol, analysis));
}

//void MetricManager::getMetrics() {}