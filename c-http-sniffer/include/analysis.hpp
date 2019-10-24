#ifndef __ANALYSIS_H__
#define __ANALYSIS_H__

#include <cstdint>
#include <netinet/in.h>
#include <sys/syscall.h>

#include "eventManager.hpp"
#include "flow.hpp"
#include "hashTable.hpp"
#include "flowHashTable.hpp"
#include "flowQueue.hpp"
#include "packetQueue.hpp"
#include "rawPacket.hpp"
#include "queue.hpp"
#include "util.hpp"
#include "ethHeader.hpp"
#include "ipHeader.hpp"
#include "tcpHeader.hpp"
#include "timer.hpp"

using namespace std; 

class Analysis {
    private:

    public:
        int flow_req;
        int flow_rsp;
        
        int pak;
        int pak_deq;
        
        int rsp_n;
        int req_n;

        int raw_req;
        int raw_rsp;

        EventManager* eventManager;

        bool debug;
        bool serverMode;
        int clientSock;

        //uint32_t stamp;

        // 0: started, 1: running, -1: stopped
        int status;
        const char* interface;
        char* ipaddress;
        uint32_t interval;
        uint32_t duration;

        int fd; 
	    int livemode;
    
        bool has_client_ip;
        bool has_client_port;
        sockaddr_in client;

        bool has_server_ip;
        bool has_server_port;
        sockaddr_in server;

        // Move this in timer
        uint32_t int_step;
        
        Queue<RawPacket>* rawPktQueue;
        PacketQueue* pktQueue;
        FlowHashTable* flowHashTable;
        FlowQueue* flowQueue;

        Analysis(bool debug, const char* interface, uint32_t interval, char* ipaddress);
        bool isServerMode();
        void start();
        void debugging_print();

        void processFlowHashTable();
        void processFlowQueue();
        void processPacketQueue();

        void printData();
        void processData(int sig);
        void printTid();
        void capture_main();
        void stop(int sig);
        void scrubbingFlowHashTable();
        Packet* packetPreprocess(char *raw_data, const struct pcap_pkthdr *pkthdr);
        void processRawPktQueue();

        void extract_data(Flow *flow);
};

#endif