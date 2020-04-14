#include "mysql.hpp"

char* MySQL::isResponse(const char *ptr, const int datalen){
    if (((unsigned int) ptr[3]>0) &&((((unsigned int)ptr[4]==0) && (datalen>7)) ||(((unsigned int)ptr[4]==254)&&( datalen<9)) || ((unsigned int)ptr[4]==255))){
		return ptr[4];
	} else return NULL;
}

char* MySQL::isRequest(const char *ptr, const int datalen) {
    if (((unsigned int) ptr[3]==0) &&((unsigned int)ptr[4]>=0) &&((unsigned int)ptr[4]<=28)){
    	return ptr[4];
	} else {
        return NULL;
    }
}

char* MySQL::getServerCmd(int m){
    return command_name[m];
}

SQL_command _mysql::parseSqlCommand(const char *data, int len){
    const char *ptr;
    int	index = 0;

    while (index < len) {
        if (*ptr == ' ')
            break;
        else {
            ptr++;
            index++;
        }
    }

    if (index != 6){
        return OTHER;
    } else {
        if (strncmp(data, "SELECT", index) == 0){
            return QUERY;
        }
        if (strncmp(data, "UPDATE", index) == 0){
            return UPDATE;
        }
        if (strncmp(data, "INSERT", index) == 0){
            return INSERT;
        }
        if (strncmp(data, "DELETE", index) == 0){
            return DELETE;
        }
    }
}

MySQL::MySQL(Analysis* analysis, char* protocolName, uint32_t protocolId) : Protocol(analysis, protocolName, protocolId) {
    ports.insert(ports.end(), {3306});
}

bool MySQL::isHeaderPacket(const char *ptr, const int datalen) {
    char *req_head_end = NULL;
    char *rsp_head_end = NULL;

    req_head_end = isRequest(ptr, datalen);
    rsp_head_end = isResponse(ptr, datalen);

    if ( (req_head_end != NULL) || (rsp_head_end != NULL)){
        return true;
    }

    return false;
}

_mysql::Request::Request(const char *data, const char *dataend, u_int32_t seq, u_int32_t nxt_seq){
    cmd = data[4];
    size = ((unsigned int) data[2] << 16) + ((unsigned int) data[1] << 8) + (unsigned int) data[0];
	seq = (unsigned int) data[3];

    sql_command = parseSqlCommand(data,size);

    ip_seq = seq;
    ip_nxt_seq = nxt_seq;
}


_mysql::Response::Response(const char *data, const char *dataend, long ack){
    cmd = data[4];
    size = ((unsigned int) data[2] << 16) + ((unsigned int) data[1] << 8) + (unsigned int) data[0];
	seq = (unsigned int) data[3];

    ip_ack = ack;
}