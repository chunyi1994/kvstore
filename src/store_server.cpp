#include "store_server.h"
#include "cmd_protocol.h"
namespace kvstore {
StoreServer::StoreServer(io_service& ioservice, std::size_t port) :
    tcp_server_(ioservice, port),
    dbs_(),
    persistence_(*this)
{
    tcp_server_.set_close_callback(
                std::bind(&StoreServer::handle_close, this, std::placeholders::_1));
    tcp_server_.set_read_callback(
                std::bind(&StoreServer::handle_read, this, std::placeholders::_1));
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

}

void StoreServer::handle_read(TcpConnection::Pointer conn) {
    if (conn->buf().length() < 3) {
        return;
    }
    unsigned short total_len = *((unsigned short*) (conn->buf().data() + POS_TOTAL_LENGTH));
    total_len = boost::asio::detail::socket_ops::network_to_host_short(total_len);
    std::size_t len = static_cast<std::size_t>(total_len);
    if (conn->buf().length() < len) {
        return;
    }
    std::string cmd = conn->buf().substr(0, len);
    conn->buf().erase(0, len);
    std::string retmsg("");
    int err = execute_cmd(cmd, retmsg);
    std::string resp = get_response(retmsg);
    if (err < 0) {
        resp[POS_TYPE] = RESP_TYPE_ERR;
    } else {
        resp[POS_TYPE] = RESP_TYPE_OK;

    }
    conn->send(resp);
    handle_read(conn);
    //process_cmd(conn, len);
}

int StoreServer::execute_cmd(const std::string &cmd, std::string &retmsg)
{
    int req_type = static_cast<int>(cmd[POS_TYPE]);
    int err = -1;
    switch (req_type) {
    case REQ_TYPE_PUT :
        err = execute_cmd_put(cmd, retmsg);
        LOG_DEBUG<<"execute_cmd_put";
        if (err == 0) { persistence_.record(cmd, dbs_.db_current_id()); }
        return err;
    case REQ_TYPE_GET:
        err =  execute_cmd_get(cmd, retmsg);
        LOG_DEBUG<<"execute_cmd_get";
        return err;
    case REQ_TYPE_DEL:
        err =  execute_cmd_del(cmd, retmsg);
        LOG_DEBUG<<"execute_cmd_del";
        if (err == 0) { persistence_.record(cmd, dbs_.db_current_id()); }
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

int StoreServer::execute_cmd_put(const std::string &cmd, std::string &retmsg) {
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

    err = dbs_.put(std::move(key), std::move(value));
    if (err < 0) {
        //todo
        LOG_DEBUG<<"err3";
        retmsg = "error";
        return -3;
    }
    retmsg = "ok";
    return 0;
}

int StoreServer::execute_cmd_del(const std::string &cmd, std::string &retmsg) {
    std::string key;
    int err = get_key(cmd, cmd.length(), key);
    if (err < 0) {
        //todo
        LOG_DEBUG<<"err1";
        retmsg = "error";
        return -1;
    }
    err = dbs_.del(std::move(key));
    if (err < 0) {
        //todo
        LOG_DEBUG<<"err2";
        retmsg = "error";
        return -2;
    }

    retmsg = "ok";
    return 0;
}

int StoreServer::execute_cmd_get(const std::string &cmd, std::string &retmsg) {
    std::string key;
    int err = get_key(cmd, cmd.length(), key);
    if (err < 0) {
        //todo
        LOG_DEBUG<<"err1";
        retmsg = "error";
        return -1;
    }
    std::string value;
    err = dbs_.get(std::move(key), value);
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

