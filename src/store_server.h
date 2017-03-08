#ifndef STORESERVER_H
#define STORESERVER_H
#include <unordered_map>
#include <map>
#include "tcp_server.h"
#include "cmd_recoder.h"
#include "persistence.h"
#include "conn_data.h"
namespace kvstore {

class StoreServer {
    friend class Recovery;
public:
    StoreServer(io_service& ioservice, std::size_t port);
public:
    int execute_cmd(const std::string& cmd, std::size_t db, std::string& retmsg);
    void init();
private:
    void handle_close(TcpConnection::Pointer conn);
    void handle_read(TcpConnection::Pointer conn, std::size_t bytes_transfered);
    void handle_new_conn(TcpConnection::Pointer conn);
    int execute_cmd_put(const std::string& cmd, std::size_t db, std::string& retmsg);
    int execute_cmd_get(const std::string& cmd, std::size_t db, std::string& retmsg);
    int execute_cmd_del(const std::string& cmd, std::size_t db, std::string& retmsg);
     int execute_cmd_use(const std::string& cmd, TcpConnection::Pointer conn,std::string& retmsg);

    KVDatabase& db();
private:

private:
    TcpServer tcp_server_;
    std::map<TcpConnection::Pointer, ConnData> conns_;
    KVDatabase dbs_;
    Persistence persistence_;
};

}//namespace
#endif // STORESERVER_H



