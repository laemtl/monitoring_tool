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
#include "analysis.hpp" 

// These defines come from asm-generic/siginfo.h, are necessary, but are not included by signal.h.
#define sigev_notify_thread_id	_sigev_un._tid

#define PERIOD_SIG SIGRTMIN
#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN

void stop_analysis(void* a) {
	Analysis* analysis = (Analysis*) a;
	analysis->stop();
	pthread_exit(NULL);
}

void processData(void* a) {
	Analysis* analysis = (Analysis*) a;
	analysis->int_step++;

	for (auto protocol = begin (analysis->protocols); protocol != end (analysis->protocols); ++protocol) {
		// The completed flow are processed by extract_data every seconds
		// We process the ones in the hash table using the following function 	
		(*protocol)->flowHashProcess();
		(*protocol)->eventManager->timerExpired->notify();
	}
	 
	if(analysis->int_step < analysis->interval) return;

	for (auto protocol = begin (analysis->protocols); protocol != end (analysis->protocols); ++protocol) {
		(*protocol)->eventManager->intervalExpired->notify();
	}

	analysis->int_step = 0;
}

void create_timer(long start, long interval, Analysis* analysis, void (*callback) (void*)) {
	sigset_t mask;
	int sig;
	timer_t timerid;
	struct itimerspec its;
	struct sigevent se;
	siginfo_t si;
	
	its.it_value.tv_sec = start;
	its.it_value.tv_nsec = 0;
	its.it_interval.tv_sec = interval;
	its.it_interval.tv_nsec = 0;

	//while(analysis->status < 1) 
	//	nanosleep((const struct timespec[]){{0, 20000000L}}, NULL);

	// by default, SIGCHLD is set to be ignored so unless we happen
    // to be blocked on sigwaitinfo() at the time that SIGCHLD
    // is set on us we will not get it.  To fix this, we simply
    // register a signal handler.  Since we've masked the signal
    // above, it will not affect us.  At the same time we will make
    // it a queued signal so that if more than one are set on us,
    // sigwaitinfo() will get them all.
	//struct sigaction sa;
	// Install timer_handler as the signal handler for SIGALRM.
	//memset (&sa, 0, sizeof (sa));
	//sa.sa_handler = &stop_analysis;
	//sigaction (PERIOD_SIG, &sa, NULL);
	
	se.sigev_notify = SIGEV_THREAD;
	//se.sigev_signo = PERIOD_SIG;

	//se.sigev_notify_thread_id = gettid();
	se.sigev_notify_function = callback;
  	se.sigev_value.sival_ptr = (void*)analysis;
	se.sigev_notify_attributes = NULL;

	/* Bloque le signal PERIOD_SIG pour le thread appelant uniquement */
	/*if (sigemptyset(&mask) ||
	    sigaddset(&mask, PERIOD_SIG) ||
	    sigprocmask(SIG_BLOCK, &mask, NULL))
		pthread_exit((void *) 10);*/

	if (timer_create(CLOCK_REALTIME, &se, &timerid) == -1) {
		error("Error with timer_create\n");
	}
	
	if (timer_settime(timerid, 0, &its, NULL)) {
		error("Error with timer_settime\n");
	}

	while (!analysis->isStopped()) usleep(100);

		/*if ((sig = sigwaitinfo(&mask, &si)) < 0) {
			error("Error with sigwaitinfo\n");
		}
			
		if (sig != PERIOD_SIG) {
			// printf("signal: %d", sig);
			continue;
		}
		
		callback(analysis);*/

	timer_delete(timerid);
}

void start_duration_timer(Analysis* analysis) {
	create_timer(analysis->duration, 0, analysis, stop_analysis);
}

void start_timer(Analysis* analysis) {
	pthread_t timer_duration;
    if(analysis->duration > 0) {	    
	    pthread_create(&timer_duration, NULL, (void*)start_duration_timer, analysis);
		pthread_detach(timer_duration);
    }

	// start an interval timer wich fires every seconds to compute rates
	create_timer(1, 1, analysis, processData);
}