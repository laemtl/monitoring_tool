#include "mysql.hpp"

char* MySQL::isResponse(const char *ptr, const int datalen){
    if (((unsigned int) ptr[3]>0) /*&&((((unsigned int)ptr[4]==0) && (datalen>7)) ||(((unsigned int)ptr[4]==254)&&( datalen<9)) || ((unsigned int)ptr[4]==255))*/){
        return &(ptr[3]);
	} else {
        return NULL;
    }
}

char* MySQL::isRequest(const char *ptr, const int datalen) {
    if ((unsigned int)ptr[3]==0 /*&&((unsigned int)ptr[4]>=0) &&((unsigned int)ptr[4]<=28)*/){
        return &(ptr[3]);
	} else {
        return NULL;
    }
}

int _mysql::Request::parseMethod(const char *data, int linelen) {
    if (strncmp(data, "SELECT", 6) == 0 || strncmp(data, "select", 6) == 0){
        methodCode = 2;
    } else if (strncmp(data, "UPDATE", 6) == 0 || strncmp(data, "update", 6) == 0){
        methodCode = 3;
    } else if (strncmp(data, "INSERT", 6) == 0 || strncmp(data, "insert", 6) == 0){
        methodCode = 1;
    } else if (strncmp(data, "DELETE", 6) == 0 || strncmp(data, "delete", 6) == 0){
        methodCode = 4;
    } else {
        methodCode = 0;
    }

    return methodCode;
}

MySQL::MySQL(Analysis* analysis, char* protocolName, uint32_t protocolId) : Protocol(analysis, protocolName, protocolId) {
    serverPorts.push_back(3306);

    metrics.push_back(new Rst(this, analysis));
    metrics.push_back(new ReqRate(this, analysis));
    metrics.push_back(new Tp(this, analysis));
    metrics.push_back(new TpRev(this, analysis));
    metrics.push_back(new ReqType(this, analysis));
}

_mysql::Request::Request(const char *data, const char *dataend, char *time, u_int32_t seq, u_int32_t nxt_seq) : Request() {
    this->seq = seq;
    this->nxt_seq = nxt_seq;

    cmd = data[4];
    size = ((unsigned int) data[2] << 16) + ((unsigned int) data[1] << 8) + (unsigned int) data[0];
	//seq = (unsigned int) data[3];
    sql_command = parseMethod(&(data[5]), size);
}

_mysql::Response::Response(const char *data, const char *dataend, long ack){
    cmd = data[4];
    size = ((unsigned int) data[2] << 16) + ((unsigned int) data[1] << 8) + (unsigned int) data[0];
	//seq = (unsigned int) data[3];
    //ip_ack = ack;
}

//int _mysql::Response::parseStatus(const char *line, int len) {}
//bool _mysql::Response::hasErrorStatus() {}

_mysql::Request::~Request() {
    if(time != NULL)
        free(time);
}

_mysql::Response::~Response() {
}

_protocol::Request* MySQL::getRequest(const char *data, const char *dataend, char* time, u_int32_t seq, u_int32_t nxt_seq) {
    return new Request(data, dataend, time, seq, nxt_seq);
}

_protocol::Response* MySQL::getResponse(const char *data, const char *dataend, long ack) {
    return new Response(data, dataend, ack);
}

_mysql::Request::Request() {
    serverCommandName.push_back("NONE");
    serverCommandName.push_back("SLEEP");
    serverCommandName.push_back("QUIT");
    serverCommandName.push_back("INIT_DB");
    serverCommandName.push_back("QUERY");
    serverCommandName.push_back("FIELD_LIST");
    serverCommandName.push_back("CREATE_DB");
    serverCommandName.push_back("DROP_DB");
    serverCommandName.push_back("REFRESH");
    serverCommandName.push_back("SHUTDOWN");
    serverCommandName.push_back("STATISTICS");
    serverCommandName.push_back("PROCESSLIST");
    serverCommandName.push_back("CONNECT");
    serverCommandName.push_back("KILL");
    serverCommandName.push_back("DEBUG");
    serverCommandName.push_back("PING");
    serverCommandName.push_back("TIME");
    serverCommandName.push_back("DELAYED_INSERT");
    serverCommandName.push_back("CHANGE_USER");
    serverCommandName.push_back("PREPARE");
    serverCommandName.push_back("EXECUTE");
    serverCommandName.push_back("LONG_DATA");
    serverCommandName.push_back("CLOSE_STMT");
    serverCommandName.push_back("RESET_STMT");
    serverCommandName.push_back("FETCH");
    serverCommandName.push_back("Binlog Dump");
    serverCommandName.push_back("Table Dump");
    serverCommandName.push_back("Connect Out");
    serverCommandName.push_back("Register Slave");
    serverCommandName.push_back("Set option");

    methodsName.push_back("OTHER");
    methodsName.push_back("INSERT");
    methodsName.push_back("SELECT");
    methodsName.push_back("UPDATE");
    methodsName.push_back("DELETE");
}

_mysql::Response::Response() {
}