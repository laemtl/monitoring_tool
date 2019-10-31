#include <stdlib.h>
#include <inttypes.h>

#include <iostream>
#include <string>
#include <stdio.h>

#include "util.h"
#include "attr.h"
#include "hash_table.h"
#include "cf_list.h"
#include "metric.hpp"

#define CFL_SIZE 100
#define MIN_FREQ (double)1/CFL_SIZE

class MetricCumDistr: public Metric2
{
	protected:
        string name;
		hash_t ht;
		cf_list cfl;

    public:
		//MetricCumDistr();
		~MetricCumDistr();
		void print();
};