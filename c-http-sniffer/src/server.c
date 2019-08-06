#include "server.h"
#include "data.h"

int sfd = 0;
extern int pak;
extern int pak_deq;
extern int rsp_n;
extern int req_n;

/*void set_server_mode(BOOL status) {
	server_mode = status;
}*/

/* decode a varint and stick results in value */
uint64_t decode_varint(int sock) {
	uint64_t value = 0, shifter = 0;   
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

/* Write the given message to the output stream, followed by
* the delimiter. Precondition: buf[] is at least msgSize.
* Returns -1 on any error.
*/
int send_data(Result* result) {
	Analysis__Data msg = ANALYSIS__DATA__INIT;
	void *buf;                 // Buffer to store serialized data
	uint64_t msg_len;          // Length of serialized data

	if(result->client_sock == NULL) {
		printf("Socket is not defined\n");
		return EXIT_FAILURE;
	}

	int varint_len_len;

	msg.netint = result->interface;
	
	msg.has_rstavg = 1;
	msg.rstavg = result->rst_avg;

	msg.has_rstmin = 1;
	msg.rstmin = result->rst_min;

	msg.has_rstmax = 1;
	msg.rstmax = result->rst_max;

	msg.has_errrate = 1;
	msg.errrate = result->err_rate;

	msg.has_errratemin = 1;
	msg.errratemin = result->err_rate_min;

	msg.has_errratemax = 1;
	msg.errratemax = result->err_rate_max;

	msg.has_reqrate = 1;
	msg.reqrate = result->req_rate;

	msg.has_reqratemin = 1;
	msg.reqratemin = result->req_rate_min;

	msg.has_reqratemax = 1;
	msg.reqratemax = result->req_rate_max;

	msg.has_connrate = 1;
	msg.connrate = result->conn_rate;

	msg.has_connratemin = 1;
	msg.connratemin = result->conn_rate_min;

	msg.has_connratemax = 1;
	msg.connratemax = result->conn_rate_max;

	/* */
	Analysis__Conn **conns;
	int c1 = result->conn_tl.count;
	conns = MALLOC(Analysis__Conn*, c1);

	int i;
	for(i = 0; i < c1; i++) {
		conns[i] = MALLOC(Analysis__Conn, 1);
		analysis__conn__init(conns[i]);
		conns[i]->ip = ((Addr*)result->conn_tl.list[i])->ip;
		conns[i]->port = ((Addr*)result->conn_tl.list[i])->port;;
	} 
	msg.n_conns = c1;
	msg.conns = conns;

	/* */
	Analysis__Freq **client;
	int c2 = result->client.count;
	client = MALLOC(Analysis__Freq*, c2); 
	
	int j;
	for(j = 0; j < c2; j++) {
		client[j] = MALLOC(Analysis__Freq, 1);
		analysis__freq__init(client[j]);
		client[j]->name = result->client.list[j].name;
		client[j]->freq = result->client.list[j].c_freq;
	}
	msg.n_client = c2;
	msg.client = client;

	/* */
	Analysis__Freq **req_path;
	int c3 = result->req_path.count;
	req_path = MALLOC(Analysis__Freq*, c3); 
	
	int k;
	for(k = 0; k < c3; k++) {
		req_path[k] = MALLOC(Analysis__Freq, 1);
		analysis__freq__init(req_path[k]);
		req_path[k]->name = result->req_path.list[k].name;
		req_path[k]->freq = result->req_path.list[k].c_freq;
	}
	msg.n_req_path = c3;
	msg.req_path = req_path;

	/* */
	Analysis__Freq **req_method;
	int c4 = result->req_method.count;
	req_method = MALLOC(Analysis__Freq*, c4); 
	
	int l;
	for(l = 0; l < c4; l++) {
		req_method[l] = MALLOC(Analysis__Freq, 1);
		analysis__freq__init(req_method[l]);
		req_method[l]->name = result->req_method.list[l].name;
		req_method[l]->freq = result->req_method.list[l].c_freq;
	}
	msg.n_req_method = c4;
	msg.req_method = req_method;

	/* */
	Analysis__Freq **req_type;
	int c5 = result->req_type.count;
	req_type = MALLOC(Analysis__Freq*, c5); 
	
	int m;
	for(m = 0; m < c5; m++) {
		req_type[m] = MALLOC(Analysis__Freq, 1);
		analysis__freq__init(req_type[m]);
		req_type[m]->name = result->req_type.list[m].name;
		req_type[m]->freq = result->req_type.list[m].c_freq;
	}
	msg.n_req_type = c5;
	msg.req_type = req_type;

	/* */
	Analysis__Freq **rsp_status;
	int c6 = result->rsp_status.count;
	rsp_status = MALLOC(Analysis__Freq*, c6); 
	
	int n;
	for(n = 0; n < c6; n++) {
		rsp_status[n] = MALLOC(Analysis__Freq, 1);
		analysis__freq__init(rsp_status[n]);
		rsp_status[n]->name = result->rsp_status.list[n].name;
		rsp_status[n]->freq = result->rsp_status.list[n].c_freq;
	}
	msg.n_rsp_status = c6;
	msg.rsp_status = rsp_status;


	msg_len = analysis__data__get_packed_size(&msg);
	
	// Max size of varint_len is 10 bytes
	buf = malloc(10 + msg_len);

	// Convert msg_len to a varint
	varint_len_len = encode_varint(buf, msg_len);	
	//fprintf(stderr, "Varint size: %d\n", varint_len_len); // See the length of varint_len_len

	analysis__data__pack(&msg, buf + varint_len_len);
	
	//fprintf(stderr, "Writing %d serialized bytes\n", varint_len_len + msg_len); // See the length of message
	if(send(result->client_sock, buf, varint_len_len + msg_len, MSG_NOSIGNAL) < 0) {
		fprintf(stderr, "Error sending response\n");
		fprintf(stderr, "Errno %d\n", errno);
		pthread_exit(NULL);
	} 

	free(buf); // Free the allocated serialized buffer
	
	for(i = 0; i < c1; i++) free(conns[i]); 
	free(conns);

	for(j = 0; j < c2; j++) free(client[j]);
	free(client);
	
	for(k = 0; k < c3; k++) free(req_path[k]);
	free(req_path);
	
	for(l = 0; l < c4; l++) free(req_method[l]);
	free(req_method);
	
	for(m = 0; m < c5; m++) free(req_type[m]);
	free(req_type);
	
	for(n = 0; n < c6; n++) free(rsp_status[n]);
	free(rsp_status);
 	
	return 0;
}

/*
 * This will handle connection for each client
 * */
void* connection_handler(int *socket) {
	int status;
	uint64_t msg_len;
	Analysis__Init *init;
	
	//Receive a message from client
	//do {
		// Read the prefix length (varint)
		msg_len = decode_varint(*socket);
		printf("msg_len: %d\n", msg_len);

		if (msg_len > 0) {
			char *msg = (char*) calloc(msg_len+1, sizeof(char));
			
			// Read the message
			if (recv(*socket, msg, msg_len, 0) != msg_len) {
				fprintf(stderr, "Framing error: expected %d, read less\n", msg_len);
				return EXIT_FAILURE;
			}

			// Unpack the message using protobuf-c.
			init = analysis__init__unpack(NULL, msg_len, msg);	
			if (init == NULL) {
				fprintf(stderr, "error unpacking incoming message\n");
				return EXIT_FAILURE;
			}
			
			// display the message's fields.
			printf("Received: interval: %d\n", init->interval);  // required field
			printf("Received: duration: %d\n", init->duration);  // required field

			for (unsigned i = 0; i < init->n_netint; i++) { // Iterate through all repeated string
				printf ("netInt: %s\n\n", init->netint[i]);
		
				Data* data = init_data();
				data->server_mode = TRUE;
				data->client_sock = *socket;
				data->interval = init->interval;
				data->duration = init->duration;
				data->interface = init->netint[i];
			
				start_analysis(NULL, data);
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
        perror("recv failed\n");
    }

	//Free the socket pointer    
    close(*socket);

	printf("Socket was closed \n \n \n");
	free(socket);
}

/*void sigpipe_handler() {
	printf("Warning ! Connection close.\n");

	pthread_cancel(pthread_t thread);

	// add close connection functionnality
	//signal(SIG_IGN);
}*/

void onExit(int signum) {
	stop_server();

	printf("pak: %d \n", pak);
    printf("dpak: %d \n", pak_deq);
	printf("reqn: %d \n", req_n);
	printf("rspn: %d \n", rsp_n);

	exit(signum);
}

void stop_server() {
	close(sfd);
}

void start_server() {
	int client_sock, client_len, s, *new_sock;
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
			error("ERROR opening socket");
			continue;
		}
		
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0) {
			error("setsockopt(SO_REUSEADDR) failed");	
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
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }
	
	freeaddrinfo(result);	/* No longer needed */
	
	signal(SIGINT, onExit);
  	signal(SIGTERM, onExit);

	/*struct sigaction act;
	memset (&act, '\0', sizeof(act));
 	/* Use the sa_sigaction field because the handles has two additional parameters */
	/*act.sa_sigaction = &sigpipe_handler;
 
	/* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
	/*act.sa_flags = SA_SIGINFO;
 
	if (sigaction(SIGPIPE, &act, NULL) < 0) {
		perror ("sigaction");
		return 1;
	}*/


	listen(sfd, 5);
    printf("Server started: %s:%s\n",hostname,portname);
	client_len = sizeof(struct sockaddr_in);

	while(client_sock = accept(sfd, (struct sockaddr *) &client_addr, (socklen_t*)&client_len)) {
		printf("Connection accepted\n");
		printf("Client address: %d \n", client_sock);
		
        new_sock = (int*) malloc(sizeof(int));
		*new_sock = client_sock;
		
		printf("New connection from client %d \n", *new_sock );
        if( pthread_create( &sniffer_thread, NULL, connection_handler, new_sock) < 0) {
            perror("Could not create thread\n");
            return EXIT_FAILURE;
        }

        //Now join the thread , so that we dont terminate before the thread
        printf("Handler assigned\n");

		pthread_detach(sniffer_thread);
	}

	if (client_sock < 0) {
		if (errno != EINTR)
			syslog(LOG_ERR, "accept failed with errno = %d\n", errno);
    }

	return EXIT_SUCCESS;
}




















/** Added Functionality: Sending packets to remote node.
 *  Following code opens socket on given IP address and port to send
 *  packets to be logged. It will connect to the remote node, then
 *  pass the file descriptor to the given function in the
 *  src/io.c file, where the http pairs of each flow will have their information
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
			printf("Error creating socket \n");
			exit(1);
	}
	int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (connect(fd,res->ai_addr,res->ai_addrlen) != 0){
			printf("Connection failed.\n");
	}
}   
