#ifndef __METRIC_H__
#define __METRIC_H__

#include "eventManager.hpp"

//class Metric2;

//#include "analysis2.hpp"
#include "analysis.pb-c.h"
#include <string>
//#include "server.h"

using namespace std;

class Analysis;
class Metric2 {
    public:  
        string name;
        string desc;
		Analysis* analysis;
        Metric2(Analysis* analysis, string name, string desc) : analysis(analysis), name(name), desc(desc) {};
        virtual void subscribe(EventManager* em) = 0;
};

#endif