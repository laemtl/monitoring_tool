#include "server.h"
#include "analysis.hpp"
#include <string.h>
#include <inttypes.h>
#include <unistd.h>

int sfd = 0;
extern int pak;
extern int pak_deq;
extern int rsp_n;
extern int req_n;

/* decode a varint and stick results in value */
size_t decode_varint(int sock) {
	size_t value = 0, shifter = 0;   
	uint8_t i = 0;
	int n = 0;

    do {  
		n = recv(sock, &i, 1, 0);
        if (n < 1) return value;
		value |= (i & 0x7F) << shifter;
		shifter += 7;                               
    } while (i & 0x80);  

	return value;                      
}    

// Encode an unsigned 64-bit varint. Returns number of encoded bytes.
// 'buffer' must have room for up to 10 bytes.
int encode_varint(uint8_t *const buffer, uint64_t value) {
    int encoded = 0;
    do {
        uint8_t next_byte = value & 0x7F;
        value >>= 7;
        if (value) next_byte |= 0x80;

        buffer[encoded++] = next_byte;
    } while (value);

    return encoded;
}

/*
 * This will handle connection for each client
 * */
void connection_handler(Config* config) {
	size_t msg_len;
	Analysis__Init *init;
	
	//Receive a message from client
	//do {
		// Read the prefix length (varint)
		msg_len = decode_varint(config->socket);
		printf("msg_len: %"PRIu64"\n", msg_len);

		if (msg_len > 0) {
			uint8_t *msg = (uint8_t*) calloc(msg_len+1, sizeof(char));
			
			// Read the message
			if (recv(config->socket, msg, msg_len, 0) != msg_len) {
				char m[100];
				snprintf(m, 100, "Framing error: expected %"PRIu64", read less\n", msg_len);
				error(m);
			}

			// Unpack the message using protobuf-c.
			init = analysis__init__unpack(NULL, msg_len, msg);	
			if (init == NULL) {
				error("error unpacking incoming message\n");
			}
			
			// display the message's fields.
			printf("Received: interval: %d\n", init->interval);  // required field
			printf("Received: duration: %d\n", init->duration);  // required field
			
			//printf("Received: active_metric: %d\n", init->activemetric);  // required field
			//printf("Received: client IP: %s\n", ip_ntos(init->clientip));
			//printf("Received: client port: %d\n", init->clientport);

			//printf("Received: server IP: %s\n", ip_ntos(init->serverip));
			//printf("Received: server port: %d\n", init->serverport);


			for (unsigned i = 0; i < init->n_netints; i++) { // Iterate through all repeated netInt
				printf ("netInt: %s\n\n", init->netints[i]->id);
				
				Analysis* analysis = new Analysis(config->socket, init->netints[i]->id, init->interval, init->duration, true, config->debug);
			
				for (unsigned j = 0; j < init->netints[i]->protocols; j++) { // Iterate through all repeated protocols
					
					Protocol* protocol;
					if(init->netints[i]->protocols[j]->id == "HTTP") {
						protocol = new Http(analysis);
					} else if(init->netints[i]->protocols[j]->id == "MEMCACHED") {
						protocol = new MemCached(analysis);
					}
					
					protocol->activeMetrics(init->netints[i]->protocols[j]->activemetrics);
					analysis->protocols.push_back(protocol);
					
					if(init->netints[i]->protocols[j]->client->has_ip) {
						analysis->setClientIp(init->netints[i]->protocols[j]->client->ip);
					}
					
					if(init->netints[i]->protocols[j]->client->has_ports) {
						analysis->setClientPort(init->netints[i]->protocols[j]->client->ports);
					}

					if(init->netints[i]->protocols[j]->server->has_ip) {
						analysis->setServerIp(init->netints[i]->protocols[j]->server->ip);
					}
					
					if(init->netints[i]->protocols[j]->server->has_ports) {
						analysis->setServerPort(init->netints[i]->protocols[j]->server->ports);
					}
				}

				start_analysis(NULL, analysis);
			}

			// Free the unpacked message
			analysis__init__free_unpacked(init, NULL);
			free(msg);
		}
				
    //} while(msg_len > 0);
	
    if(msg_len == 0) {
        puts("Client disconnected\n");
        fflush(stdout);
    } else if(msg_len == -1) {
        error("recv failed\n");
    }

	//Free the socket pointer    
    close(config->socket);
	printf("Socket was closed \n \n \n");
}

/*void sigpipe_handler() {
	printf("Warning ! Connection close.\n");

	pthread_cancel(pthread_t thread);

	// add close connection functionnality
	//signal(SIG_IGN);
}*/

void stop_server() {
	close(sfd);
}

void onExit(int signum) {
	stop_server();

	printf("pak: %d \n", pak);
    printf("dpak: %d \n", pak_deq);
	printf("reqn: %d \n", req_n);
	printf("rspn: %d \n", rsp_n);

	exit(signum);
}

void start_server(void* debug) {
	int client_sock, client_len, s;
    struct sockaddr_in client_addr;

	char* hostname = "0.0.0.0";
	char* portname = "3000";

	struct addrinfo hints, *result, *rp;
	pthread_t sniffer_thread;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	hints.ai_flags = AI_ADDRCONFIG;
	
	s = getaddrinfo(hostname, portname, &hints, &result);
	if (s != 0 ){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
	}

	/*  getaddrinfo() returns a list of address structures.
		Try each address until we successfully bind(2).
		If socket(2) (or bind(2)) fails, we (close the socket
		and) try the next address. */

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd < 0) {
			perror("ERROR opening socket");
			continue;
		}
		
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0) {
			perror("setsockopt(SO_REUSEADDR) failed");	
		}
    		
		#ifdef SO_REUSEPORT
			if (setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT, &(int){ 1 }, sizeof(int)) < 0) {
				perror("setsockopt(SO_REUSEPORT) failed");
			}
		#endif

		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;	/* Success */

		printf("Bind Error code: %d\n", errno);

		close(sfd);
	}

	if (rp == NULL) {	/* No address succeeded */
        error("Could not bind\n");
    }
	
	freeaddrinfo(result);	/* No longer needed */
	
	signal(SIGINT, onExit);
  	signal(SIGTERM, onExit);

	listen(sfd, 5);
    printf("Server started: %s:%s\n",hostname,portname);
	client_len = sizeof(struct sockaddr_in);

	while(client_sock = accept(sfd, (struct sockaddr*)&client_addr, (socklen_t*)&client_len)) {
		printf("Connection accepted\n");
		printf("Client address: %d \n", client_sock);
		
       	Config* config = new Config(client_sock, *(bool*)debug);

        if( pthread_create( &sniffer_thread, NULL, (void*)connection_handler, config) < 0) {
            error("Could not create thread\n");
        }

        //Now join the thread , so that we dont terminate before the thread
		pthread_detach(sniffer_thread);
	}

	if (client_sock < 0) {
		if (errno != EINTR)
			syslog(LOG_ERR, "accept failed with errno = %d\n", errno);
    }
}

/** Added Functionality: Sending packets to remote node.
 *  Following code opens socket on given IP address and port to send
 *  packets to be logged. It will connect to the remote node, then
 *  pass the file descriptor to the given function in the
 *  src/io.c file, where the pairs of each flow will have their information
 *  extracted, parsed into string, and sent.
**/
void start_log(char* ipaddress) {
    char* hostname="172.16.1.10";
	if (ipaddress != NULL) hostname=ipaddress;
	char* portname = "6009";

    struct addrinfo hints;
	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_DGRAM;
	hints.ai_protocol=0;
	hints.ai_flags=AI_ADDRCONFIG;

	struct addrinfo* res=0;
	printf("Sending to: IP - %s:%s entered: %s\n",hostname,portname,ipaddress);

    int err=getaddrinfo(hostname,portname,&hints,&res);
	if (err != 0 ){
		error("Error creating socket \n");
	}
	int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (connect(fd,res->ai_addr,res->ai_addrlen) != 0){
			printf("Connection failed.\n");
	}
}   