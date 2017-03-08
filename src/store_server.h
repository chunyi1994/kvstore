#ifndef STORESERVER_H
#define STORESERVER_H
#include <unordered_map>
#include <map>
#include "tcp_server.h"
#include "cmd_recoder.h"
#include "persistence.h"
#include "conn_data.h"
#include "config.h"
namespace kvstore {

class StoreServer {
    friend class Recovery;
public:
    StoreServer(io_service& ioservice, std::size_t port);
public:
    int execute_db_cmd(const std::string& cmd, std::size_t db, std::string& retmsg);
    void init();
private:
    void handle_close(TcpConnection::Pointer conn);
    void handle_read(TcpConnection::Pointer conn, std::size_t bytes_transfered);
    void handle_new_conn(TcpConnection::Pointer conn);
    int dispath_cmd(const std::string& cmd, TcpConnection::Pointer conn,std::string& retmsg);
    int execute_cmd_put(const std::string& cmd, std::size_t db, std::string& retmsg);
    int execute_cmd_get(const std::string& cmd, std::size_t db, std::string& retmsg);
    int execute_cmd_del(const std::string& cmd, std::size_t db, std::string& retmsg);
    int execute_cmd_use(const std::string& cmd, TcpConnection::Pointer conn,std::string& retmsg);
    int execute_cmd_login(const std::string& cmd, TcpConnection::Pointer conn,std::string& retmsg);
    int execute_cmd_mult(const std::string& cmd, TcpConnection::Pointer conn,std::string& retmsg);
    int execute_cmd_exec(const std::string& cmd, TcpConnection::Pointer conn,std::string& retmsg);
    int add_transaction(const std::string& cmd, TcpConnection::Pointer conn,std::string& retmsg);
    int execute_cmd_discard(const std::string& cmd, TcpConnection::Pointer conn,std::string& retmsg);
    int execute_cmd_watch(const std::string& cmd, TcpConnection::Pointer conn,std::string& retmsg);
    KVDatabase& db();
private:

private:
    TcpServer tcp_server_;
    std::map<TcpConnection::Pointer, ConnData> conns_;
    KVDatabase dbs_;
    Config  config_;
    Persistence persistence_;
};

}//namespace
#endif // STORESERVER_H



