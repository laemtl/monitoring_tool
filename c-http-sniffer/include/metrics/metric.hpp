#ifndef __METRIC_H__
#define __METRIC_H__

#include "eventManager.hpp"
#include "analysis.pb-c.h"
#include <string>
#include "protocol.hpp"

//#include "server.h"

using namespace std;
class Analysis;
class Metric {
    public:  
        string name;
        string desc;
		Protocol* protocol;
        Analysis* analysis;
        Metric(Protocol* protocol, Analysis* analysis, string name, string desc) : analysis(analysis), protocol(protocol), name(name), desc(desc) {};
        virtual void subscribe(EventManager* em) = 0;
};

#include "analysis.hpp"

#endif