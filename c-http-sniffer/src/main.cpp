//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include <pthread.h>
//#include <assert.h>
#include <unistd.h> /* getopt() */
//#include <pcap.h>
//#include <errno.h>
//#include <signal.h> 
//#include <inttypes.h>

//#include "rawPacket.hpp"
//#include "flow.hpp"
#include "util.hpp"
#include "server.hpp"
#include "analysis.hpp"
//#include "flowHashTable.hpp"
//#include "timer.hpp"


//int GP_CAP_FIN = 0; /* Flag for offline PCAP sniffing */

/**
 * Help function to print usage information.
 */
void
print_usage(const char* pro_name){
	printf("Usage: %s -i interface [-f tracefile] [-p ipaddress]\n", pro_name);
}

/*void sigintHandler(int sig_num) { 
	signal(SIGINT, sigintHandler); 
    printf("pak: %d \n", pak);
	printf("dpak: %d \n", pak_deq);
	printf("reqn: %d \n", req_n);
	printf("rspn: %d \n", rsp_n);

	printf("raw_req: %d \n", raw_req);
	printf("raw_rsp: %d \n", raw_rsp);
	printf("flow_req: %d \n", flow_req);
	printf("flow_rsp: %d \n", flow_rsp);

	exit(0);
}*/ 

/**
 * Main portal of http-sniffer
 */
int main(int argc, char *argv[]){
	char* interface = NULL;
	char* ipaddress = NULL;
	int opt;
	bool debug = false;

	//signal(SIGINT, sigintHandler);
	printf("My process ID : %d\n", getpid());

   	// Parse arguments
	while((opt = getopt(argc, argv, ":i:f:o:p:h:d")) != -1){
		switch(opt){
			case 'h':
				print_usage(argv[0]); return (1);
			case 'i':
				interface = optarg; break;
			case 'p':
				ipaddress = optarg; break;
			case 'd':
				debug = true; break;
		}
	}
    
    // Interface is NULL - RUN in server mode	
	if (interface == NULL){
		/* Start server thread */

		pthread_t job_server;
		pthread_create(&job_server, NULL, (THREADFUNCPTR) &start_server, (void*)debug);
		pthread_join(job_server, NULL);
	
	// Interface is provided - RUN in app mode
	} else {		
		Analysis* analysis = new Analysis(debug, interface, 5, ipaddress);
	}

	return 0;
}