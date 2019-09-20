#ifndef RESULT_H
#define RESULT_H

#include "cf_list.h"

typedef struct _result Result;
struct _result {
    char* interface;
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
    
    double tp_avg;
    u_int32_t tp_min;
    u_int32_t tp_max;

    double tp_rev_avg;
    u_int32_t tp_rev_min;
    u_int32_t tp_rev_max;

    double conn_rate;
    double conn_rate_min;
    double conn_rate_max;

    cf_list client;
    cf_list req_path;
    cf_list req_method;
    cf_list req_type;
    cf_list rsp_status;
};

#endif
