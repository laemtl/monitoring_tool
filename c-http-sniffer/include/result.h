#ifndef RESULT_H
#define RESULT_H

#include "top_list.h"
#include "cf_list.h"

typedef struct _result Result;
struct _result {
    const char* interface;
    int client_sock;

	double rst_avg;
    double rst_min;
    double rst_max;

	double err_rate;
    double err_rate_min;
    double err_rate_max;

    double req_rate;
    double req_rate_min;
    double req_rate_max;
    
    double tp;

    double conn_rate;
    double conn_rate_min;
    double conn_rate_max;
    
    top_list conn_tl;

    cf_list client;
    cf_list req_path;
    cf_list req_method;
    cf_list req_type;
    cf_list rsp_status;
};

#endif