#include "server.h"
#include "data.h"
#include <string.h>
#include <inttypes.h>
#include <unistd.h>

int sfd = 0;
extern int pak;
extern int pak_deq;
extern int rsp_n;
extern int req_n;

/*void set_server_mode(BOOL status) {
	server_mode = status;
}*/

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

/* Write the given message to the output stream, followed by
* the delimiter. Precondition: buf[] is at least msgSize.
* Returns -1 on any error.
*/
int send_data(Result* result) {
	Data* data = {0};
	get_data(&data);
	
	Analysis__Data msg = ANALYSIS__DATA__INIT;
	void *buf;                 // Buffer to store serialized data
	uint64_t msg_len;          // Length of serialized data
	
	//Analysis__Conn **conns;
	Analysis__Freq **client;
	Analysis__Freq **req_path;
	Analysis__Freq **req_method;
	Analysis__Freq **req_type;
	Analysis__Freq **rsp_status;

	int j, k, l, m, n, 
	c2, c3, c4, c5, c6; 
	
	if(result->client_sock < 0) {
		error("Socket is not defined\n");
	}

	int varint_len_len;

	msg.netint = result->interface;

	msg.has_rstavg = 0;
	msg.has_rstmin = 0;
	msg.has_rstmax = 0;
	if(data->rst.active) {
		msg.has_rstavg = 1;
		msg.rstavg = result->rst_avg;

		msg.has_rstmin = 1;
		msg.rstmin = result->rst_min;

		msg.has_rstmax = 1;
		msg.rstmax = result->rst_max;
	}

	msg.has_errrate = 0;
	msg.has_errratemin = 0;
	msg.has_errratemax = 0;
	if(data->err_rate.active) {
		msg.has_errrate = 1;
		msg.errrate = result->err_rate;

		msg.has_errratemin = 1;
		msg.errratemin = result->err_rate_min;

		msg.has_errratemax = 1;
		msg.errratemax = result->err_rate_max;
	}

	msg.has_reqrate = 0;
	msg.has_reqratemin = 0;
	msg.has_reqratemax = 0;
	if(data->req_rate.active) {
		msg.has_reqrate = 1;
		msg.reqrate = result->req_rate;

		msg.has_reqratemin = 1;
		msg.reqratemin = result->req_rate_min;

		msg.has_reqratemax = 1;
		msg.reqratemax = result->req_rate_max;
	}

	msg.has_tpavg = 0;
	msg.has_tpmin = 0;
	msg.has_tpmax = 0;
	if(data->tp.active) {
		msg.has_tpavg = 1;
		msg.tpavg = result->tp_avg;

		msg.has_tpmin = 1;
		msg.tpmin = result->tp_min;

		msg.has_tpmax = 1;
		msg.tpmax = result->tp_max;
	}

	msg.has_tprevavg = 0;
	msg.has_tprevmin = 0;
	msg.has_tprevmax = 0;
	if(data->tp_rev.active) {
		msg.has_tprevavg = 1;
		msg.tprevavg = result->tp_rev_avg;

		msg.has_tprevmin = 1;
		msg.tprevmin = result->tp_rev_min;

		msg.has_tprevmax = 1;
		msg.tprevmax = result->tp_rev_max;
	}

	msg.has_connrate = 0;
	msg.has_connratemin = 0;
	msg.has_connratemax = 0;
	if(data->conn_rate.active) {
		msg.has_connrate = 1;
		msg.connrate = result->conn_rate;

		msg.has_connratemin = 1;
		msg.connratemin = result->conn_rate_min;

		msg.has_connratemax = 1;
		msg.connratemax = result->conn_rate_max;
	}

	if(data->client_active) {
		/* */
		c2 = result->client.count;
		client = MALLOC(Analysis__Freq*, c2); 
		
		for(j = 0; j < c2; j++) {
			client[j] = MALLOC(Analysis__Freq, 1);
			analysis__freq__init(client[j]);
			client[j]->name = result->client.list[j].name;
			client[j]->freq = result->client.list[j].c_freq;
		}
		msg.n_client = c2;
		msg.client = client;
	}

	if(data->req_path_active) {
		/* */
		c3 = result->req_path.count;
		req_path = MALLOC(Analysis__Freq*, c3); 
		
		for(k = 0; k < c3; k++) {
			req_path[k] = MALLOC(Analysis__Freq, 1);
			analysis__freq__init(req_path[k]);
			req_path[k]->name = result->req_path.list[k].name;
			req_path[k]->freq = result->req_path.list[k].c_freq;
		}
		msg.n_req_path = c3;
		msg.req_path = req_path;
	}

	if(data->req_method_active) {
		/* */
		c4 = result->req_method.count;
		req_method = MALLOC(Analysis__Freq*, c4); 
	
		for(l = 0; l < c4; l++) {
			req_method[l] = MALLOC(Analysis__Freq, 1);
			analysis__freq__init(req_method[l]);
			req_method[l]->name = result->req_method.list[l].name;
			req_method[l]->freq = result->req_method.list[l].c_freq;
		}
		msg.n_req_method = c4;
		msg.req_method = req_method;
	}

	if(data->req_type_active) {
		/* */
		c5 = result->req_type.count;
		req_type = MALLOC(Analysis__Freq*, c5); 
		
		for(m = 0; m < c5; m++) {
			req_type[m] = MALLOC(Analysis__Freq, 1);
			analysis__freq__init(req_type[m]);
			req_type[m]->name = result->req_type.list[m].name;
			req_type[m]->freq = result->req_type.list[m].c_freq;
		}
		msg.n_req_type = c5;
		msg.req_type = req_type;
	}

	if(data->rsp_status_active) {
		/* */
		c6 = result->rsp_status.count;
		rsp_status = MALLOC(Analysis__Freq*, c6); 
		
		for(n = 0; n < c6; n++) {
			rsp_status[n] = MALLOC(Analysis__Freq, 1);
			analysis__freq__init(rsp_status[n]);
			rsp_status[n]->name = result->rsp_status.list[n].name;
			rsp_status[n]->freq = result->rsp_status.list[n].c_freq;
		}
		msg.n_rsp_status = c6;
		msg.rsp_status = rsp_status;
	}

	msg_len = analysis__data__get_packed_size(&msg);
	
	// Max size of varint_len is 10 bytes
	buf = malloc(10 + msg_len);

	// Convert msg_len to a varint
	varint_len_len = encode_varint(buf, msg_len);	
	//fprintf(stderr, "Varint size: %d\n", varint_len_len); // See the length of varint_len_len

	analysis__data__pack(&msg, buf + varint_len_len);
	
	//fprintf(stderr, "Writing %d serialized bytes\n", varint_len_len + msg_len); // See the length of message
	if(send(result->client_sock, buf, varint_len_len + msg_len, MSG_NOSIGNAL) < 0) {
		error("Error sending response\n");
	} 

	free(buf); // Free the allocated serialized buffer
	
	/*if(data->conn_rate.active) {
		for(i = 0; i < c1; i++) free(conns[i]); 
		free(conns);
	}*/

	if(data->client_active) {
		for(j = 0; j < c2; j++) free(client[j]);
		free(client);
	}

	if(data->req_path_active) {
		for(k = 0; k < c3; k++) free(req_path[k]);
		free(req_path);
	}

	if(data->req_method_active) {
		for(l = 0; l < c4; l++) free(req_method[l]);
		free(req_method);
	}

	if(data->req_type_active) {
		for(m = 0; m < c5; m++) free(req_type[m]);
		free(req_type);
	}
	
	if(data->rsp_status_active) {
		for(n = 0; n < c6; n++) free(rsp_status[n]);
		free(rsp_status);
	}

	return 0;
}

/*
 * This will handle connection for each client
 * */
void connection_handler(int *socket) {
	size_t msg_len;
	Analysis__Init *init;
	
	//Receive a message from client
	//do {
		// Read the prefix length (varint)
		msg_len = decode_varint(*socket);
		printf("msg_len: %"PRIu64"\n", msg_len);

		if (msg_len > 0) {
			uint8_t *msg = (uint8_t*) calloc(msg_len+1, sizeof(char));
			
			// Read the message
			if (recv(*socket, msg, msg_len, 0) != msg_len) {
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
			//printf("Received: top_client_cnt: %d\n", init->topclientcnt);  // required field
			printf("Received: active_metric: %d\n", init->activemetric);  // required field
			printf("Received: client IP: %s\n", ip_ntos(init->clientip));
			printf("Received: client port: %d\n", init->clientport);

			printf("Received: server IP: %s\n", ip_ntos(init->serverip));
			printf("Received: server port: %d\n", init->serverport);

			BOOL rst_active = (init->activemetric & (1<<0));
			BOOL req_rate_active = (init->activemetric & (1<<1));
			BOOL err_rate_active = (init->activemetric & (1<<2));
			BOOL tp_active = (init->activemetric & (1<<3));
			BOOL tp_rev_active = (init->activemetric & (1<<4));
			BOOL conn_rate_active = (init->activemetric & (1<<5));
			BOOL client_active = (init->activemetric & (1<<6));
			BOOL req_path_active = (init->activemetric & (1<<7));
			BOOL req_method_active = (init->activemetric & (1<<8));
			BOOL req_type_active = (init->activemetric & (1<<9));
			BOOL rsp_status_active = (init->activemetric & (1<<10));

			for (unsigned i = 0; i < init->n_netint; i++) { // Iterate through all repeated string
				printf ("netInt: %s\n\n", init->netint[i]);
		
				Data* data = init_data();
				data->server_mode = TRUE;
				data->client_sock = *socket;
				data->interval = init->interval;
				data->duration = init->duration;
				data->interface = init->netint[i];

				if(init->has_clientip) {
					data->has_client_ip = TRUE;
					data->client.ip = init->clientip;
				
					if(init->has_clientport) {
						data->has_client_port = TRUE;
						data->client.port = init->clientport;
					}
				}

				if(init->has_serverip) {
					data->has_server_ip = TRUE;
					data->server.ip = init->serverip;
				
					if(init->has_serverport) {
						data->has_server_port = TRUE;
						data->server.port = init->serverport;
					}
				}

				//data->conn_ht.tl.size = init->topclientcnt;

				data->rst.active = rst_active;
				data->req_rate.active = req_rate_active;
				data->err_rate.active = err_rate_active;
				data->tp.active = tp_active;
				data->tp_rev.active = tp_rev_active;
				data->conn_rate.active = conn_rate_active;

				data->client_active = client_active;
				data->req_path_active = req_path_active;
				data->req_method_active = req_method_active;
				data->req_type_active = req_type_active;
				data->rsp_status_active = rsp_status_active;
			
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
        error("recv failed\n");
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
		
        new_sock = (int*) malloc(sizeof(int));
		*new_sock = client_sock;
		
		printf("New connection from client %d \n", *new_sock );
        if( pthread_create( &sniffer_thread, NULL, (void*)connection_handler, new_sock) < 0) {
            error("Could not create thread\n");
        }

        //Now join the thread , so that we dont terminate before the thread
        printf("Handler assigned\n");

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
		error("Error creating socket \n");
	}
	int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (connect(fd,res->ai_addr,res->ai_addrlen) != 0){
			printf("Connection failed.\n");
	}
}   
