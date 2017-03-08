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
public:
    typedef std::function<void(boost::system::error_code& error)> ConnectCallback;
    typedef std::pair<int, std::string>  Result;
public:
    StoreClient(boost::asio::io_service& ioservice);
    void connect(const std::string& addr, const std::string& port);

    Result use(std::size_t db);
    Result put(const std::string& key, const std::string& value);
    Result get(const std::string& key);
    Result del(const std::string& key);
    Result login(const std::string& password);
    Result mult();
    Result exec();
    Result discard();
    Result watch(const std::string& key);
private:
    boost::asio::io_service& ioservice_;
    boost::asio::ip::tcp::socket socket_;
};

}//namespace

#endif // STORECLIENT_H
