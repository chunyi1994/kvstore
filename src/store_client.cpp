#include "store_client.h"
#include "cmd_protocol.h"
namespace kvstore{

StoreClient::StoreClient(boost::asio::io_service& ioservice) :
    ioservice_(ioservice),
    socket_(ioservice)
{

}

void StoreClient::connect(const std::string& addr,const std::string&  port) {
    //boost::system::error_code error;
    boost::asio::ip::tcp::resolver resolver(ioservice_);
    LOG_INFO<<"正在尝试连接"<<addr<<":"<<port;
    boost::asio::ip::tcp::resolver::query query(addr, port);
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    boost::asio::connect(socket_, endpoint_iterator);
}

std::string get_req(std::string key, std::string value) {
    std::string msg = std::string("xxxxx") + key + value;
    Len len;
    len.us = static_cast<unsigned short> (msg.length());
    len.us = boost::asio::detail::socket_ops::host_to_network_short(len.us);
    msg[POS_TOTAL_LENGTH] = len.c[0];
    msg[POS_TOTAL_LENGTH + 1] = len.c[0 + 1];

    len.us = static_cast<unsigned short> (key.length());
    len.us = boost::asio::detail::socket_ops::host_to_network_short(len.us);
    msg[POS_KEY_LENGTH] = len.c[0];
    msg[POS_KEY_LENGTH + 1] = len.c[0 + 1];
    return msg;
}

void StoreClient::put(std::string key, std::string value)
{
    std::string msg = get_req(std::move(key), std::move(value));
    msg[POS_TYPE] = REQ_TYPE_PUT;

    boost::asio::write(socket_, boost::asio::buffer(msg));

    std::vector<char> buffer(3);
    size_t reply_length = boost::asio::read(socket_,boost::asio::buffer(buffer));
    std::string response(buffer.data(), reply_length);
    unsigned short s = *((unsigned short*) (response.data() + POS_TOTAL_LENGTH));
    s = boost::asio::detail::socket_ops::network_to_host_short(s);
    if (s <= 1 + 2) {
        return;
    }
    std::vector<char> buffer2(s - 1 - 2);
    reply_length = boost::asio::read(socket_,boost::asio::buffer(buffer2));
    std::string retmsg{buffer2.data(), reply_length};
    if (response[POS_TYPE] == RESP_TYPE_OK) {
        LOG_DEBUG<<"success:"<<retmsg;
    } else {
        LOG_DEBUG<<"fail:"<<retmsg;
    }

}

void StoreClient::get(std::string key)
{
    std::string msg = get_req(std::move(key), std::string(""));
    msg[POS_TYPE] = REQ_TYPE_GET;
    boost::asio::write(socket_, boost::asio::buffer(msg));

    std::vector<char> buffer(3);
    size_t reply_length = boost::asio::read(socket_,boost::asio::buffer(buffer));
    std::string response(buffer.data(), reply_length);
    unsigned short s = *((unsigned short*) (response.data() + POS_TOTAL_LENGTH));
    s = boost::asio::detail::socket_ops::network_to_host_short(s);
    if (s <= 1 + 2) {
        return;
    }
    std::vector<char> buffer2(s - 1 - 2);
    reply_length = boost::asio::read(socket_,boost::asio::buffer(buffer2));
    std::string retmsg{buffer2.data(), reply_length};
    if (response[POS_TYPE] == RESP_TYPE_OK) {
        LOG_DEBUG<<"success:"<<retmsg;
    } else {
        LOG_DEBUG<<"fail:"<<retmsg;
    }

}

void StoreClient::del(std::string key)
{
    std::string msg = get_req(std::move(key), std::string(""));
    msg[POS_TYPE] = REQ_TYPE_DEL;
    boost::asio::write(socket_, boost::asio::buffer(msg));

    std::vector<char> buffer(3);
    size_t reply_length = boost::asio::read(socket_,boost::asio::buffer(buffer));
    std::string response(buffer.data(), reply_length);
    unsigned short s = *((unsigned short*) (response.data() + POS_TOTAL_LENGTH));
    s = boost::asio::detail::socket_ops::network_to_host_short(s);
    if (s <= 1 + 2) {
        return;
    }
    std::vector<char> buffer2(s - 1 - 2);
    reply_length = boost::asio::read(socket_,boost::asio::buffer(buffer2));
    std::string retmsg{buffer2.data(), reply_length};
    if (response[POS_TYPE] == RESP_TYPE_OK) {
        LOG_DEBUG<<"success:"<<retmsg;
    } else {
        LOG_DEBUG<<"fail:"<<retmsg;
    }
}


}//namespace
