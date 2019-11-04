#ifndef __METRIC_CUM_DISTR_H__
#define __METRIC_CUM_DISTR_H__

#include <stdlib.h>
#include <inttypes.h>

#include <iostream>
#include <string>
#include <stdio.h>

#include "util.h"
#include "hashTable.hpp"
#include "cf_list.h"
#include "metric.hpp"
#include "server.h"
#include "analysis2.hpp"

#define CFL_SIZE 100
#define MIN_FREQ (double)1/CFL_SIZE

class MetricCumDistr: public Metric2, public Observer
{
	protected:
        hash_t ht;
		cf_list cfl;

    public:
		void subscribe(EventManager* em);
		MetricCumDistr(Analysis* analysis, string name, string desc);
		~MetricCumDistr();
		void cflUpdate(Hash* ht);
		void cflUpdate(int* array, int size);
		virtual void cflAdd(Hashable* elem, int cnt) = 0;
		virtual void cflAdd(int i, int cnt) = 0;
		void print();
		void sendMsg();
};

#endif