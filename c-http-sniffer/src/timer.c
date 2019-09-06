#include <time.h>
#include <signal.h> 
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <errno.h>

#include "data.h" 

// These defines come from asm-generic/siginfo.h, are necessary, but are not included by signal.h.
#define sigev_notify_function	_sigev_un._sigev_thread._function
#define sigev_notify_attributes	_sigev_un._sigev_thread._attribute
#define sigev_notify_thread_id	_sigev_un._tid

#define PERIOD_SIG SIGRTMIN
#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN

void stop_analysis(int tid) {
	Data* data = {0};
	get_data(&data);

	data->status = -1;

	//timer_delete(tid);
	pthread_exit(NULL);
}

static void errExit(msg) {
	do { 
		perror(msg); exit(EXIT_FAILURE); 
	} while (0);
}

static pid_t gettid(void) {
	return syscall(SYS_gettid);
}

void create_timer(long start, long interval, int* status, void (*callback) (int)) {
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

	while(status < 1) 
		nanosleep((const struct timespec[]){{0, 20000000L}}, NULL);

	// by default, SIGCHLD is set to be ignored so unless we happen
    // to be blocked on sigwaitinfo() at the time that SIGCHLD
    // is set on us we will not get it.  To fix this, we simply
    // register a signal handler.  Since we've masked the signal
    // above, it will not affect us.  At the same time we will make
    // it a queued signal so that if more than one are set on us,
    // sigwaitinfo() will get them all.
	struct sigaction sa;
	// Install timer_handler as the signal handler for SIGALRM.
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = &stop_analysis;
	sigaction (PERIOD_SIG, &sa, NULL);
	
	se.sigev_notify = SIGEV_THREAD_ID;
	se.sigev_signo = PERIOD_SIG;
	se.sigev_notify_thread_id = gettid();
  	se.sigev_value.sival_ptr = callback;

	/* Bloque le signal PERIOD_SIG pour le thread appelant uniquement */
	if (sigemptyset(&mask) ||
	    sigaddset(&mask, PERIOD_SIG) ||
	    sigprocmask(SIG_BLOCK, &mask, NULL))
		pthread_exit((void *) 10);

	if (timer_create(CLOCK_REALTIME, &se, &timerid)) {
		printf("Error with timer_create\n");
		pthread_exit((void *) 11);
	}
	
	if (timer_settime(timerid, 0, &its, NULL)) {
		printf("Error with timer_settime\n");
		pthread_exit((void *) 12);
	}

	while (1) {
		if ((sig = sigwaitinfo(&mask, &si)) < 0) {
			printf("Error with sigwaitinfo, errno %d\n", errno);
			pthread_exit((void *) 13);
		}
			
		if (sig != PERIOD_SIG) {
			// printf("signal: %d", sig);
			continue;
		}
		
		callback(timerid);
	}
}

void start_duration_timer(Data* data) {
	thread_init(data);
	create_timer(data->duration, 0, &(data->status), stop_analysis);
}

void start_timer(Data* data) {
	thread_init(data);
        
    pthread_t timer_duration;
    if(data->duration > 0) {	    
	    pthread_create(&timer_duration, NULL, (void*)start_duration_timer, data);
		pthread_detach(timer_duration);
    }

	// start an interval timer wich fires every seconds to compute rates
	start_interval_timer(1, &(data->status));
}

void start_interval_timer(uint32_t interval, int* status) {
	create_timer(interval, interval, status, process_data);
}