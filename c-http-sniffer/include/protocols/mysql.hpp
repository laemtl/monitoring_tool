#ifndef __MYSQL_H__
#define __MYSQL_H__

#include <vector>
#include <map>
#include "protocol.hpp"

namespace _mysql {
    enum Server_command {
        COM_SLEEP, COM_QUIT, COM_INIT_DB, COM_QUERY, COM_FIELD_LIST,
        COM_CREATE_DB, COM_DROP_DB, COM_REFRESH, COM_SHUTDOWN, COM_STATISTICS,
        COM_PROCESS_INFO, COM_CONNECT, COM_PROCESS_KILL, COM_DEBUG, COM_PING,
        COM_TIME, COM_DELAYED_INSERT, COM_CHANGE_USER, COM_BINLOG_DUMP,
        COM_TABLE_DUMP, COM_CONNECT_OUT, COM_REGISTER_SLAVE,
        COM_STMT_PREPARE, COM_STMT_EXECUTE, COM_STMT_SEND_LONG_DATA, COM_STMT_CLOSE,
        COM_STMT_RESET, COM_SET_OPTION, COM_STMT_FETCH,
        
        /* Must be last */
        COM_END
    };

    static std::vector<char*> serverCommandName;

    class MySQL : public _protocol::Protocol {
        public:
            std::vector<char*> cmd;

            MySQL(Analysis* analysis, char* protocolName, uint32_t protocolId);
            bool isHeaderPacket(const char *ptr, const int datalen);
            char* isRequest(const char *p, const int datalen);	    /* If the packet carries MySQL request data */
            char* isResponse(const char *p, const int datalen);	    /* If the packet carries MySQL response data */
            _protocol::Request* getRequest(const char *data, const char *dataend, char* time, u_int32_t seq, u_int32_t nxt_seq);
            _protocol::Response* getResponse(const char *data, const char *dataend, long ack);
    };

    class Request : public RequestMethod {
        public:
            Server_command  cmd;
            int             sql_command;
            u_int32_t       size;

            Request();
            Request(const char *data, const char *dataend, char *time, u_int32_t seq, u_int32_t nxt_seq);
            ~Request();
            int parseMethod(const char *data, int linelen);

    };

    class Response : public _protocol::Response {
        public:
            Server_command  cmd;
            u_int32_t       size;
            long            ip_ack;

            Response();
            Response(const char *data, const char *dataend, long ack);
            ~Response();
    };
};

using namespace _mysql;

#endif