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
        /* don't forget to update const char *command_name[] in sql_parse.cc */

        /* Must be last */
        COM_END
    };

    static std::map<int, Server_command> command;

    static const char *command_name[] = {
        "SLEEP", "QUIT", "INIT_DB", "QUERY", "FIELD_LIST", "CREATE_DB",
        "DROP_DB", "REFRESH", "SHUTDOWN", "STATISTICS", "PROCESSLIST",
        "CONNECT","KILL","DEBUG","PING","TIME","DELAYED_INSERT","CHANGE_USER",
        "Binlog Dump","Table Dump",  "Connect Out", "Register Slave",
        "PREPARE", "EXECUTE", "LONG_DATA", "CLOSE_STMT",
        "RESET_STMT", "Set option", "FETCH",
        "Error"                                       // Last command number
    };

    enum SQL_command{
        INSERT, QUERY, UPDATE, DELETE, OTHER
    };

    SQL_command parseSqlCommand(const char *data, int len);

    class MySQL : public _protocol::Protocol {
        public:
            std::vector<int> ports;
            std::vector<char*> cmd;

            MySQL(Analysis* analysis, char* protocolName);
            bool isHeaderPacket(const char *ptr, const int datalen);
            char* isRequest(const char *p, const int datalen);	    /* If the packet carries MySQL request data */
            char* isResponse(const char *p, const int datalen);	    /* If the packet carries MySQL response data */

            int extractPair(Flow* flow, bool closed);
            //_protocol::Request* getRequest(const char *data, const char *dataend, u_int32_t seq, u_int32_t nxt_seq);
            //_protocol::Response* getResponse(const char *data, const char *dataend, long ack);
            char* getServerCmd(int m);
    };

    class Request : public _protocol::Request {
        public:
            Server_command  cmd;
            SQL_command sql_command;
            u_int32_t   size;
            u_int32_t   seq;

            u_int32_t   ip_seq;
            u_int32_t   ip_nxt_seq;

            Request();
            Request(const char *data, const char *dataend, u_int32_t seq, u_int32_t nxt_seq);
            ~Request();

    };

    class Response : public _protocol::Response {
        public:
            Server_command  cmd;
            u_int32_t   size;
            u_int32_t   seq;

            long ip_ack;

            Response();
            Response(const char *data, const char *dataend, long ack);
            ~Response();
    };
};

using namespace _mysql;

#endif