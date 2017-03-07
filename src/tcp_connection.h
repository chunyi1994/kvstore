#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H
#include <memory>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <vector>
#include <functional>
#include <string>

namespace kvstore {

using namespace boost::asio::ip;
using boost::asio::io_service;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
    typedef std::shared_ptr<TcpConnection> Pointer;
    typedef std::weak_ptr<TcpConnection> WeakPointer;
    typedef boost::array<char, 4096> ConnBuffer;
    typedef std::function<void(Pointer)> CloseCallback;
    typedef std::function<void(Pointer)> ReadCallback;
public:
    explicit TcpConnection(io_service& io_service);
    ~TcpConnection();
    std::string& buf();
    std::string read_all();
    void start();
    static Pointer create(boost::asio::io_service& io_service);
    tcp::socket& socket();
    void set_close_callback(const CloseCallback& cb);
    void set_read_callback(const ReadCallback& cb);
    void send(const std::string& msg);
    void send(const std::vector<char>& msg);
    void shutdown();
    bool connecting() const;

private:
    void handle_write(const boost::system::error_code &ec, std::size_t bytes_transfered);
    void handle_read(const boost::system::error_code &ec, std::size_t bytes_transfered);
    void handle_close();

private:
    ConnBuffer readbuf_;
    std::string buf_;
    tcp::socket socket_;
    bool is_connecting_;
    CloseCallback close_callback_;
    ReadCallback read_callback_;
};

} // namespace
#endif // TCP_CONNECTION_H
