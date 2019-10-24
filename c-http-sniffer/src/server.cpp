#include "server.hpp"
#include "analysis.hpp"
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/util/delimited_message_util.h>

int sfd = 0;
extern int pak;
extern int pak_deq;
extern int rsp_n;
extern int req_n;

/*void set_server_mode(BOOL status) {
	server_mode = status;
}*/

/* decode a varint and stick results in value */
/* size_t decode_varint(int sock) {
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
*/   

// Encode an unsigned 64-bit varint. Returns number of encoded bytes.
// 'buffer' must have room for up to 10 bytes.
/*int encode_varint(uint8_t *const buffer, uint64_t value) {
    int encoded = 0;
    do {
        uint8_t next_byte = value & 0x7F;
        value >>= 7;
        if (value) next_byte |= 0x80;

        buffer[encoded++] = next_byte;
    } while (value);

    return encoded;
}*/

/* Write the given message to the output stream, followed by
* the delimiter. Precondition: buf[] is at least msgSize.
* Returns -1 on any error.
*/

// TODO : encoding on class type

/*bool writeDelimitedTo(
    const google::protobuf::MessageLite& message,
    google::protobuf::io::ZeroCopyOutputStream* rawOutput
) {
  // We create a new coded stream for each message.  Don't worry, this is fast.
  google::protobuf::io::CodedOutputStream output(rawOutput);

  // Write the size.
  const int size = message.ByteSize();
  output.WriteVarint32(size);

  uint8_t* buffer = output.GetDirectBufferForNBytesAndAdvance(size);
  if (buffer != NULL) {
    // Optimization:  The message fits in one buffer, so use the faster
    // direct-to-array serialization path.
    message.SerializeWithCachedSizesToArray(buffer);
  } else {
    // Slightly-slower path when the message is multiple buffers.
    message.SerializeWithCachedSizes(&output);
    if (output.HadError()) return false;
  }

  return true;
}*/



/*bool writeDelimitedTo(
    const google::protobuf::MessageLite& message,
    google::protobuf::io::ZeroCopyOutputStream* rawOutput
) {

  // We create a new coded stream for each message.  Don't worry, this is fast.
  google::protobuf::io::CodedOutputStream output(rawOutput);

  // Write the size.
  const int size = message.ByteSize();
  output.WriteVarint32(size);

  uint8_t* buffer = output.GetDirectBufferForNBytesAndAdvance(size);
  if (buffer != NULL) {
    // Optimization:  The message fits in one buffer, so use the faster
    // direct-to-array serialization path.
    message.SerializeWithCachedSizesToArray(buffer);
  } else {
    // Slightly-slower path when the message is multiple buffers.
    message.SerializeWithCachedSizes(&output);
    if (output.HadError()) return false;
  }

  return true;
}*/

// ParseDelimitedFromCodedStream

int send_data(int client_sock, const google::protobuf::MessageLite& msg) {
	//uint8_t *buf;                 // Buffer to store serialized data
	//uint64_t msg_len;          // Length of serialized data

	if(client_sock < 0) {
		error("Socket is not defined\n");
	}

	/*ArrayOutputStream::ArrayOutputStream(
        void * data,
        int size,
        int block_size = -1)*/


	//.SerializeToOstream(&output)

	//int varint_len_len;
	/*google::protobuf::io::CodedOutputStream output = 
	SerializeDelimitedToCodedStream(msg, output);
	SerializeDelimitedToOstream(msg, std::ostream* output);
	SerializeDelimitedToZeroCopyStream(
    const MessageLite& message, io::ZeroCopyOutputStream* output);*/


	int size = msg.ByteSize() + 4;
	//int size = msgSize + HEADER_SIZE; // HEADER_SIZE is 4
	char *buffer = new char[size];
	google::protobuf::io::ArrayOutputStream aos(buffer, size);
	google::protobuf::io::CodedOutputStream coded_output(&aos);
	//coded_output.WriteVarint32(msg.ByteSize());

	google::protobuf::util::SerializeDelimitedToCodedStream(msg, &coded_output);
	int tsize = coded_output.ByteCount();

	//msg_len = analysis__data__get_packed_size(&msg);
	
	// Max size of varint_len is 10 bytes
	//buf = (uint8_t*) malloc(10 + msg_len);

	// Convert msg_len to a varint
	//varint_len_len = encode_varint(buf, msg_len);	
	//fprintf(stderr, "Varint size: %d\n", varint_len_len); // See the length of varint_len_len

	//analysis__data__pack(&msg, buf + varint_len_len);
	
	//fprintf(stderr, "Writing %d serialized bytes\n", varint_len_len + msg_len); // See the length of message
	if(send(client_sock, buffer, tsize, MSG_NOSIGNAL) < 0) {
		error("Error sending response\n");
	} 

	//free(buf); // Free the allocated serialized buffer

	return 0;
}

/*
 * This will handle connection for each client
 * */
void connection_handler(Analysis analysis) {
	size_t msg_len;
	analysis::Init *init;
	
	//Receive a message from client
	//do {
		// Read the prefix length (varint)
		msg_len = decode_varint(*socket);
		printf("msg_len: %"PRIu64"\n", msg_len);

		if (msg_len > 0) {
			uint8_t *msg = (uint8_t*) calloc(msg_len+1, sizeof(char));
			
			// Read the message
			if (recv(analysis.clientSock, msg, msg_len, 0) != msg_len) {
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
				Analysis a = analysis.clone();
				printf ("netInt: %s\n\n", init->netint[i]);

				//Data* data = init_data();
				//data->server_mode = TRUE;
				//data->client_sock = *socket;
				//data->interval = init->interval;
				//data->duration = init->duration;
				//data->interface = init->netint[i];

				a.setServerMode(true);
				a.setInterval(init->interval);
				a.setDuration(init->duration);
				a.setInterface(init->netint[i]);
				
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

				data->rst.setActive(rst_active);
				data->req_rate.setActive(req_rate_active);
				data->err_rate.setActive(err_rate_active);
				data->tp.setActive(tp_active);
				data->tp_rev.setActive(tp_rev_active);
				data->conn_rate.setActive(conn_rate_active);

				data->client_active = client_active;
				data->req_path_active = req_path_active;
				data->req_method_active = req_method_active;
				data->req_type_active = req_type_active;
				data->rsp_status_active = rsp_status_active;
			
				start_analysis(NULL);
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

void start_server(bool debug) {
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
		
		Analysis* analysis = new Analysis(debug, client_sock);
		//analysis.setSocket(client_sock);
		//analysis.setDebug(client_sock);
		
        //new_sock = (int*) malloc(sizeof(int));
		//*new_sock = client_sock;
		
		printf("New connection from client %d \n", *new_sock );
        if( pthread_create( &sniffer_thread, NULL, (THREADFUNCPTR) &connection_handler, analysis) < 0) {
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
