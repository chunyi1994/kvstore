#include "tcp_server.h"

#include <functional>
#include <utility>

#include "logging/logger.h"

namespace kvstore {

TcpServer::TcpServer(io_service &ioservice, size_t port) :
    acceptor_(ioservice, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
    conns_()
{
    start_accept();
}


void TcpServer::start_accept() {
    TcpConnection::Pointer conn = TcpConnection::create(acceptor_.get_io_service());
    acceptor_.async_accept(conn->socket(), std::bind(
                               &TcpServer::handle_accept, this, conn, std::placeholders::_1));
}

void TcpServer::handle_accept(TcpConnection::Pointer conn,  const boost::system::error_code &ec) {
    if (!ec) {
        conns_.insert(conn);
        conn->set_close_callback(
                    std::bind(&TcpServer::handle_close, this, std::placeholders::_1));
        conn->set_read_callback(
                    std::bind(&TcpServer::handle_read, this,  std::placeholders::_1));
        conn->start();
    } else {
        LOG_ERROR<<ec.message();
    }
    start_accept();
}

void TcpServer::handle_close(TcpConnection::Pointer conn) {
    auto iter = conns_.find(conn);
    assert(iter != conns_.end());
    conns_.erase(iter);
    if (close_callback_) {
        close_callback_(conn);
    }
}

void TcpServer::handle_read(TcpConnection::Pointer conn) {
    if (read_callback_) {
        read_callback_(conn);
    }
}

void TcpServer::set_close_callback(TcpConnection::CloseCallback cb) {
    close_callback_ = std::move(cb);
}

void TcpServer::set_read_callback(TcpConnection::ReadCallback cb) {
    read_callback_ = std::move(cb);
}


} //namespace
