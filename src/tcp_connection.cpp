#include "tcp_connection.h"

#include <functional>

#include "logging/logger.h"
namespace kvstore {

TcpConnection::TcpConnection(io_service &io_service) :
    readbuf_(),
    buf_(),
    socket_(io_service),
    is_connecting_(false)
{
}

TcpConnection::~TcpConnection() {
    LOG_DEBUG<<"~TcpConnection";
}

std::string &TcpConnection::buf() {
    return buf_;
}

std::string TcpConnection::read_all()
{
    std::string ret = buf_;
    buf_ = "";
    return ret;
}

void TcpConnection::start() {
    is_connecting_ = true;
    socket_.async_read_some(boost::asio::buffer(readbuf_), std::bind(
                                &TcpConnection::handle_read, this, std::placeholders::_1, std::placeholders::_2));
}

TcpConnection::Pointer TcpConnection::create(io_service &io_service) {
    return std::make_shared<TcpConnection>(io_service);
}

tcp::socket &TcpConnection::socket() {
    return socket_;
}

void TcpConnection::set_close_callback(const TcpConnection::CloseCallback &cb) {
    close_callback_ = cb;
}

void TcpConnection::set_read_callback(const TcpConnection::ReadCallback &cb) {
    read_callback_ = cb;
}

void TcpConnection::send(const std::string &msg) {
    boost::asio::async_write(socket_, boost::asio::buffer(msg), std::bind(
                                 &TcpConnection::handle_write, this, std::placeholders::_1, std::placeholders::_2));
}

void TcpConnection::send(const std::vector<char> &msg) {
    boost::asio::async_write(socket_, boost::asio::buffer(msg), std::bind(
                                 &TcpConnection::handle_write, this, std::placeholders::_1, std::placeholders::_2));
}

void TcpConnection::shutdown() {
    handle_close();
}

bool TcpConnection::connecting() const {
    return is_connecting_;
}

void TcpConnection::handle_write(const boost::system::error_code &ec, std::size_t bytes_transfered)
{}

void TcpConnection::handle_read(const boost::system::error_code &ec, std::size_t bytes_transfered) {
    if (!ec) {
        buf_.append(std::string(readbuf_.data(), bytes_transfered));
        if(read_callback_) {
            read_callback_(shared_from_this());
        }
        socket_.async_read_some(boost::asio::buffer(readbuf_), std::bind(
                                    &TcpConnection::handle_read, this, std::placeholders::_1,  std::placeholders::_2));
    } else {
        if (ec.value() == boost::asio::error::eof) {// EOF
            handle_close();
        }
    }
}

void TcpConnection::handle_close() {
    is_connecting_ = false;
    if (close_callback_) {
        close_callback_(shared_from_this());
    }
}

} // namespace

