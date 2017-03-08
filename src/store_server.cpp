#include "store_server.h"
#include "cmd_protocol.h"
#include <assert.h>
namespace kvstore {
StoreServer::StoreServer(io_service& ioservice, std::size_t port) :
    tcp_server_(ioservice, port),
    conns_(),
    dbs_(),
    config_(),
    persistence_(*this)
{
    tcp_server_.set_close_callback(
                std::bind(&StoreServer::handle_close, this, std::placeholders::_1));
    tcp_server_.set_new_conn_callback(
                std::bind(&StoreServer::handle_new_conn, this, std::placeholders::_1));
    tcp_server_.set_read_callback(
                std::bind(&StoreServer::handle_read, this, std::placeholders::_1, std::placeholders::_2));
    LOG_INFO<<"kvstore server start!";
}

void StoreServer::handle_close(TcpConnection::Pointer conn) {
    auto iter = conns_.find(conn);
    assert(iter != conns_.end());
    conns_.erase(iter);
}

void StoreServer::handle_read(TcpConnection::Pointer conn, std::size_t bytes_transfered) {
    ConnData& data = conns_[conn];
    std::string& buf = data.buf;
    //没有获得完整命令，就返回
    buf.append(std::string(conn->buf().data(), bytes_transfered));
    if (buf.length() < 3) {
        return;
    }
    std::size_t len = static_cast<std::size_t>(total_len(buf));
    if (buf.length() < len) {
        return;
    }
    std::string cmd = buf.substr(0, len);
    buf.erase(0, len);
    std::string retmsg("");
    //将命令分发到对应的处理函数中，返回结果
    int err = dispath_cmd(cmd, conn, retmsg);
    //发送response
    std::string response("");
    if (err < 0) {
        response = get_response(retmsg, RESP_TYPE_ERR);
    } else {
        response = get_response(retmsg, RESP_TYPE_OK);
        persistence_.record(cmd,  data.db);
    }
    conn->send(response);
    //尾递归，处理没有处理完的命令
    handle_read(conn, 0);
}

void StoreServer::handle_new_conn(TcpConnection::Pointer conn) {
    conns_[conn] = ConnData();
}

int StoreServer::dispath_cmd(const std::string &cmd,
                             TcpConnection::Pointer conn, std::string &retmsg)
{
    ConnData& data = conns_[conn];
    int err = 0;
    if (cmd[POS_TYPE] == REQ_TYPE_LOGIN) {
        //输入密码登陆
        return execute_cmd_login(cmd, conn, retmsg);
    }
    if (!data.login && config_.need_password()) {
        retmsg = "please enter password";
        LOG_DEBUG<<"用户未登录";
        return -1;
    }
    if (cmd[POS_TYPE] == REQ_TYPE_MULT) {
        //mult
        return execute_cmd_mult(cmd, conn, retmsg);
    }
    if (cmd[POS_TYPE] == REQ_TYPE_WATCH) {
        //watch
        return execute_cmd_watch(cmd, conn, retmsg);
    }
    if (cmd[POS_TYPE] == REQ_TYPE_EXEC) {
        //exec
        return  execute_cmd_exec(cmd, conn, retmsg);
    }
    if (cmd[POS_TYPE] == REQ_TYPE_DISCARD) {
        //discard
        return  execute_cmd_discard(cmd, conn, retmsg);
    }
    if (data.transaction.mult()) {
        //在输入了mult之后，get put del clear会进入这，加入事务
        return add_transaction(cmd, conn, retmsg);
    }
    if (cmd[POS_TYPE] == REQ_TYPE_USE) {
        //切换db
        return  execute_cmd_use(cmd, conn, retmsg);
    }
    //get put del clear
    err = execute_db_cmd(cmd, data.db, retmsg);
    return err;
}

int StoreServer::execute_db_cmd(const std::string &cmd, std::size_t db, std::string &retmsg) {
    int req_type = static_cast<int>(cmd[POS_TYPE]);
    int err = -1;
    switch (req_type) {
    case REQ_TYPE_PUT :
        err = execute_cmd_put(cmd, db, retmsg);
        LOG_DEBUG<<"execute_cmd_put";
        return err;
    case REQ_TYPE_GET:
        err =  execute_cmd_get(cmd, db, retmsg);
        LOG_DEBUG<<"execute_cmd_get";
        return err;
    case REQ_TYPE_DEL:
        err =  execute_cmd_del(cmd, db, retmsg);
        LOG_DEBUG<<"execute_cmd_del";
        return err;
    default:
        break;
    }
    retmsg = "cmd syntax error";
    return err;
}

void StoreServer::init() {
    LOG_DEBUG<<"开始恢复数据";
    persistence_.recover();
    LOG_DEBUG<<"恢复数据完成";
}

KVDatabase &StoreServer::db() {
    return dbs_;
}

int StoreServer::execute_cmd_put(const std::string &cmd, std::size_t db,std::string &retmsg) {
    std::string key;
    int err = get_key(cmd, cmd.length(), key);
    if (err < 0) {
        //todo
        LOG_DEBUG<<"err1";
        retmsg = "error";
        return -1;
    }
    std::string value;
    err = get_value(cmd, cmd.length(), key.length(), value);
    if (err < 0) {
        //todo
        LOG_DEBUG<<"err2";
        retmsg = "error";
        return -2;
    }
    err = dbs_.put(db, std::move(key), std::move(value));
    if (err < 0) {
        //todo
        LOG_DEBUG<<"err3";
        retmsg = "error";
        return -3;
    }
    retmsg = "ok";
    return 0;
}

int StoreServer::execute_cmd_del(const std::string &cmd, std::size_t db,std::string &retmsg) {
    std::string keystring;
    int err = first_item(cmd, keystring);
    if (err < 0) {
        //todo
        LOG_DEBUG<<"err1";
        retmsg = "error";
        return -1;
    }
    err = dbs_.del(db, std::move(keystring));
    if (err < 0) {
        //todo
        LOG_DEBUG<<"null";
        retmsg = "null";
        return -2;
    }
    retmsg = "ok";
    return 0;
}

int StoreServer::execute_cmd_use(const std::string &cmd,
                                 TcpConnection::Pointer conn, std::string &retmsg) {
    unsigned char c = cmd[POS_DB_ID];
    std::size_t db = static_cast<std::size_t>(c);
    if (db >= dbs_.size()) {
        retmsg = "error:number is too large";
        LOG_DEBUG<<"err";
        return -1;
    }
    retmsg = "ok";
    conns_[conn].db = db;
    return 0;
}

int StoreServer::execute_cmd_login(const std::string &cmd,
                                   TcpConnection::Pointer conn, std::string &retmsg)
{
    retmsg = "ok";
    if (!config_.need_password()) {
        return 0;
    }
    std::string password;
    int err = first_item(cmd, password);
    if (err < 0) {
        LOG_DEBUG<<"err";
        retmsg = "syntax error";
        return -1;
    }

    if (password != config_.password()) {
        LOG_DEBUG<<"err";
        retmsg = "password wrong";
        return -2;
    }
    conns_[conn].login = true;
    return 0;
}

int StoreServer::execute_cmd_mult(const std::string &cmd,
                                  TcpConnection::Pointer conn, std::string &retmsg)
{
    ConnData& data = conns_[conn];
    if (data.transaction.mult()) {
        retmsg = "error";
        return -1;
    }
    data.transaction.set_mult(true);
    retmsg = "ok";
    return 0;
}

int StoreServer::execute_cmd_exec(const std::string &cmd,
                                  TcpConnection::Pointer conn, std::string &retmsg)
{
    //todo
    ConnData& data = conns_[conn];
    int err = 0;
    for (auto &pair : data.transaction.watchs()) {
        std::string value("");
        dbs_.get(data.db, pair.first, value);
        if (value != pair.second) {
            err = -1;
            break;
        }
    }
    if (err == -1) {
        retmsg="watch value has been changed";
        return err;
    }
    std::string ret;
    retmsg = "";
    for (auto &cmd : data.transaction.cmds()) {
        err = execute_db_cmd(cmd, data.db, ret);
        retmsg = retmsg + ret + "\r\n";
        if (err < 0) {
            break;
        }
    }
    if (!retmsg.empty()) {
        retmsg.erase(retmsg.size() - 2, 2);
    }
    data.transaction.set_mult(false);
    data.transaction.clear_cmd();
    data.transaction.clear_watch();
    return err;
}

int StoreServer::add_transaction(const std::string &cmd,
                                 TcpConnection::Pointer conn, std::string &retmsg)
{
    ConnData& data = conns_[conn];
    if (cmd[POS_TYPE] == REQ_TYPE_USE) {
        retmsg = "error";
        return -1;
    }
    data.transaction.add_cmd(cmd);
    retmsg = "queued";
    return 0;
}

int StoreServer::execute_cmd_discard(const std::string &cmd,
                                     TcpConnection::Pointer conn, std::string &retmsg)
{

    ConnData& data = conns_[conn];
     if (!data.transaction.mult()) {
         retmsg = "don't need to discard";
         return -1;
     }
    data.transaction.set_mult(false);
    data.transaction.clear_cmd();
    data.transaction.clear_watch();
    retmsg = "ok";
    return 0;
}

int StoreServer::execute_cmd_watch(const std::string &cmd,
                                   TcpConnection::Pointer conn, std::string &retmsg)
{
    std::string keystring;
    int err = first_item(cmd, keystring);
    if (err < 0) {
        //todo
        LOG_DEBUG<<"cmd syntax error";
        retmsg = "cmd syntax error";
        return -1;
    }
    std::string value;
    ConnData& data = conns_[conn];
    err = dbs_.get(data.db, std::move(keystring), value);
    if (err < 0) {
        value = "";
    }
    data.transaction.add_watch(keystring, value);
    return 0;
}

int StoreServer::execute_cmd_get(const std::string &cmd, std::size_t db,std::string &retmsg) {
    std::string keystring;
    int err = first_item(cmd, keystring);
    if (err < 0) {
        //todo
        LOG_DEBUG<<"err1";
        retmsg = "error";
        return -1;
    }
    std::string value;
    err = dbs_.get(db, std::move(keystring), value);
    if (err < 0) {
        //todo
        LOG_DEBUG<<"null";
        retmsg = "null";
        return -2;
    }
    retmsg = value;
    return 0;
}


}//namespace

