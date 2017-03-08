#include "store_server.h"
#include "cmd_protocol.h"
#include <assert.h>
namespace kvstore {
StoreServer::StoreServer(io_service& ioservice, std::size_t port) :
    tcp_server_(ioservice, port),
    conns_(),
    dbs_(),
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

std::string get_response(const std::string& msg) {
    std::string resp = std::string{"xxx"}  + msg;
    Len len;
    len.us = boost::asio::detail::socket_ops::host_to_network_short(resp.size());
    resp[POS_TOTAL_LENGTH] = len.c[0];
    resp[POS_TOTAL_LENGTH + 1] = len.c[1];
    return resp;
}

void StoreServer::handle_close(TcpConnection::Pointer conn) {
    auto iter = conns_.find(conn);
    assert(iter != conns_.end());
    conns_.erase(iter);
}

void StoreServer::handle_read(TcpConnection::Pointer conn, std::size_t bytes_transfered) {
    ConnData& data = conns_[conn];
    std::string& buf = data.buf;
    buf.append(std::string(conn->buf().data(), bytes_transfered));
    if (buf.length() < 3) {
        return;
    }
    unsigned short total_len = *((unsigned short*) (buf.data() + POS_TOTAL_LENGTH));
    total_len = boost::asio::detail::socket_ops::network_to_host_short(total_len);
    std::size_t len = static_cast<std::size_t>(total_len);
    if (buf.length() < len) {
        return;
    }
    std::string cmd = buf.substr(0, len);
    buf.erase(0, len);
    std::string retmsg("");
    int err = 0;
    if (cmd[POS_TYPE] == REQ_TYPE_USE) {
        err = execute_cmd_use(cmd, conn, retmsg);
    } else {
        err = execute_cmd(cmd, data.db, retmsg);
    }

    std::string resp = get_response(retmsg);
    if (err < 0) {
        resp[POS_TYPE] = RESP_TYPE_ERR;
    } else {
        resp[POS_TYPE] = RESP_TYPE_OK;
        persistence_.record(cmd,  data.db);
    }
    conn->send(resp);
    //process_cmd(conn, len);
}

void StoreServer::handle_new_conn(TcpConnection::Pointer conn) {
    conns_[conn] = ConnData();
}

int StoreServer::execute_cmd(const std::string &cmd, std::size_t db, std::string &retmsg) {
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
    std::string key;
    int err = get_key(cmd, cmd.length(), key);
    if (err < 0) {
        //todo
        LOG_DEBUG<<"err1";
        retmsg = "error";
        return -1;
    }
    err = dbs_.del(db, std::move(key));
    if (err < 0) {
        //todo
        LOG_DEBUG<<"err2";
        retmsg = "error";
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
    conns_[conn].db = db;
    return 0;
}

int StoreServer::execute_cmd_get(const std::string &cmd, std::size_t db,std::string &retmsg) {
    std::string key;
    int err = get_key(cmd, cmd.length(), key);
    if (err < 0) {
        //todo
        LOG_DEBUG<<"err1";
        retmsg = "error";
        return -1;
    }
    std::string value;
    err = dbs_.get(db, std::move(key), value);
    if (err < 0) {
        //todo
        LOG_DEBUG<<"err2";
        retmsg = "error";
        return -2;
    }

    retmsg = value;
    return 0;
}


}//namespace

