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

static StoreClient::Result recv_response(boost::asio::ip::tcp::socket& socket) {
    std::vector<char> buffer(3);
    size_t reply_length = boost::asio::read(socket,boost::asio::buffer(buffer));
    std::string response(buffer.data(), reply_length);
    unsigned short s = *((unsigned short*) (response.data() + POS_TOTAL_LENGTH));
    s = boost::asio::detail::socket_ops::network_to_host_short(s);
    if (s <= 2) {
        return std::make_pair(-1,"");
    }
    if (s == 1 + 2) {
        return std::make_pair(response[POS_TYPE],"");
    }
    std::vector<char> buffer2(s - 1 - 2);
    reply_length = boost::asio::read(socket,boost::asio::buffer(buffer2));
    std::string retmsg{buffer2.data(), reply_length};
    return std::make_pair(response[POS_TYPE], std::move(retmsg));
}

StoreClient::Result StoreClient::use(std::size_t db) {
    std::string msg = get_use_request(db);
    boost::asio::write(socket_, boost::asio::buffer(msg));
   return recv_response(socket_);
}

StoreClient::Result StoreClient::put(const std::string& key, const std::string& value) {
    std::string msg = get_put_request(std::move(key), std::move(value));
    boost::asio::write(socket_, boost::asio::buffer(msg));
    return recv_response(socket_);
}

StoreClient::Result StoreClient::get(const std::string& key) {
    std::string msg = get_get_request(key);
    boost::asio::write(socket_, boost::asio::buffer(msg));
    return recv_response(socket_);
}

StoreClient::Result StoreClient::del(const std::string& key) {
    std::string msg = get_del_request(key);
    boost::asio::write(socket_, boost::asio::buffer(msg));
    return recv_response(socket_);
}

StoreClient::Result StoreClient::login(const std::string& password) {
    std::string msg = get_login_request(password);
    boost::asio::write(socket_, boost::asio::buffer(msg));
    return recv_response(socket_);
}

StoreClient::Result StoreClient::mult() {
    std::string msg = get_mult_request();
    boost::asio::write(socket_, boost::asio::buffer(msg));
    return recv_response(socket_);
}

StoreClient::Result StoreClient::exec() {
    std::string msg = get_exec_request();
    boost::asio::write(socket_, boost::asio::buffer(msg));
    return recv_response(socket_);
}

StoreClient::Result StoreClient::discard() {
    std::string msg = get_discard_request();
    boost::asio::write(socket_, boost::asio::buffer(msg));
    return recv_response(socket_);
}

StoreClient::Result StoreClient::watch(const std::string& key)
{
    auto msg = get_watch_request(key);
    boost::asio::write(socket_, boost::asio::buffer(msg));
    return recv_response(socket_);
}


}//namespace
