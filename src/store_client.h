#ifndef STORECLIENT_H
#define STORECLIENT_H
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <unistd.h>
#include <set>
#include <functional>

#include "tcp_connection.h"
#include "logging/logger.h"
#include "logging/log_recorder.h"

namespace kvstore {

class StoreClient {
    typedef std::function<void(boost::system::error_code& error)> ConnectCallback;
public:
    StoreClient(boost::asio::io_service& ioservice);
    void connect(const std::string& addr, const std::string& port);

    void put(std::string key, std::string value);
    void get(std::string key);
    void del(std::string key);
private:
    boost::asio::io_service& ioservice_;
    boost::asio::ip::tcp::socket socket_;
};

}//namespace

#endif // STORECLIENT_H
