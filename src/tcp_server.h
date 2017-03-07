#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <unistd.h>
#include <set>
#include <functional>

#include "tcp_connection.h"
#include "logging/logger.h"
#include "logging/log_recorder.h"
namespace kvstore {

class TcpServer  //nocopyable
{
public:
    typedef std::function<void(TcpConnection::Pointer)> ConnectionCallback;

public:
    TcpServer(io_service& ioservice, std::size_t port);
    void set_close_callback(TcpConnection::CloseCallback cb);
    void set_read_callback(TcpConnection::ReadCallback cb);

private:
    void start_accept();
    void handle_accept(TcpConnection::Pointer conn,  const boost::system::error_code &ec);
    void handle_close(TcpConnection::Pointer conn);
    void handle_read(TcpConnection::Pointer conn);

private:
    TcpServer(const TcpServer&) = delete;
    TcpServer(TcpServer&&) = delete;
    TcpServer& operator=(const TcpServer&) = delete;

private:
    boost::asio::ip::tcp::acceptor acceptor_;
    std::set<TcpConnection::Pointer> conns_;
    TcpConnection::CloseCallback close_callback_;
    TcpConnection::ReadCallback read_callback_;
};


} // namespace



#endif // TCPSERVER_H



