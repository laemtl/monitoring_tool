#ifndef __TIMER_H__
#define __TIMER_H__

#include <time.h>
#include <signal.h> 
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

class Analysis;

void stop_analysis(void* a);
void processData(void* a);
void create_timer(long start, long interval, Analysis* analysis, void (*callback) (void*));
void start_duration_timer(Analysis* analysis);
void start_timer(Analysis* analysis);

#include "analysis.hpp" 

#endif