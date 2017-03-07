#include <thread>
#include "test.h"
#include "../store_server.h"
#include "../store_client.h"
using namespace kvstore;



static void test_client_connect(boost::asio::io_service& ioservice) {
    std::thread t([&](){
        boost::asio::io_service ioservice2;
        ioservice2.run();
        StoreClient client(ioservice2);
        client.connect("127.0.0.1", "23333");
        client.put("123", "456");
        client.get("123");
        client.del("123");
        client.get("123");
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
    EXCEPT_TRUE(true);
    int x = 3;
    EXCEPT_TRUE(x == 3);
    EXCEPT_EQ(x, 3);
   // test_client_connect(ioservice);
    test_client_connect2(ioservice);
}
