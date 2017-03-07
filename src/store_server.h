#ifndef STORESERVER_H
#define STORESERVER_H
#include <unordered_map>
#include "tcp_server.h"
#include "cmd_recoder.h"
#include "persistence.h"
namespace kvstore {

class StoreServer {
public:
    StoreServer(io_service& ioservice, std::size_t port);
public:
    int execute_cmd(const std::string& cmd, std::string& retmsg);
    void init();
    KVDatabase& db();
private:
    void handle_close(TcpConnection::Pointer conn);
    void handle_read(TcpConnection::Pointer conn);
    int execute_cmd_put(const std::string& cmd, std::string& retmsg);
    int execute_cmd_get(const std::string& cmd, std::string& retmsg);
    int execute_cmd_del(const std::string& cmd, std::string& retmsg);
private:

private:
    TcpServer tcp_server_;
    KVDatabase dbs_;
    Persistence persistence_;
};

}//namespace
#endif // STORESERVER_H



