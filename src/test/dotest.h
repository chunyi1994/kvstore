#include <thread>
#include "test.h"
#include "test/test_config.h"
#include "../store_server.h"
#include "../store_client.h"
using namespace kvstore;
namespace test {

static void test_client_connect(boost::asio::io_service& ioservice) {
    std::thread t([&](){
        boost::asio::io_service ioservice2;
        ioservice2.run();
        StoreClient client(ioservice2);
        client.connect("127.0.0.1", "23333");
        client.use(2);
        client.put("123", "456");
        client.get("123");
       // client.del("123");
       // client.get("123");
    });
    t.detach();
}

static void test_client_connect2(boost::asio::io_service& ioservice) {
    std::thread t([&](){
        boost::asio::io_service ioservice2;
        ioservice2.run();
        StoreClient client(ioservice2);
        client.connect("127.0.0.1", "23333");
        client.get("123");
        client.del("123");
    });
    t.detach();
}

static void do_test(boost::asio::io_service& ioservice) {
   test_config();
   test_client_connect(ioservice);
   // test_client_connect2(ioservice);
}

}//namespace
