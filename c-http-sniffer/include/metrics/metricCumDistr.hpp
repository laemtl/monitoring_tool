#ifndef __METRIC_CUM_DISTR_H__
#define __METRIC_CUM_DISTR_H__

#include <stdlib.h>
#include <inttypes.h>
#include <string>
#include <stdio.h>
#include <iostream>

#include "hashTable.hpp"
#include "cf_list.h"
#include "metric.hpp"
#include "observer.hpp"

#define CFL_SIZE 100
#define MIN_FREQ (double)1/CFL_SIZE

class MetricCumDistr: public Metric, public Observer
{
	protected:
        Hash ht;
		cf_list cfl;

    public:
		void subscribe(EventManager* em);
		MetricCumDistr(_protocol::Protocol* protocol, Analysis* analysis, std::string name, std::string desc);
		~MetricCumDistr();
		void cflUpdate(Hash* ht);
		virtual void cflAdd(Hashable* elem, int cnt) = 0;
		void print();
		void sendMsg();
};

#include "util.h"
#include "analysis.hpp"
#include "server.h"

#endif