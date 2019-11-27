#ifndef __METRIC_CUM_DISTR_H__
#define __METRIC_CUM_DISTR_H__

#include <stdlib.h>
#include <inttypes.h>

#include <string>
#include <stdio.h>
#include <iostream>
#include "util.h"
#include "hashTable.hpp"
#include "cf_list.h"
#include "analysis.hpp"
#include "metric.hpp"
#include "server.h"

#define CFL_SIZE 100
#define MIN_FREQ (double)1/CFL_SIZE

class MetricCumDistr: public Metric, public Observer
{
	protected:
        Hash ht;
		cf_list cfl;

    public:
		void subscribe(EventManager* em);
		MetricCumDistr(Protocol* protocol, Analysis* analysis, string name, string desc);
		~MetricCumDistr();
		void cflUpdate(Hash* ht);
		void cflUpdate(int* array, int size);
		virtual void cflAdd(Hashable* elem, int cnt) = 0;
		virtual void cflAdd(int i, int cnt) = 0;
		void print();
		void sendMsg();
};

#endif