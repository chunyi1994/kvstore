#ifndef TEST_CLIENT
#define TEST_CLIENT
#include "test.h"
#include <thread>
#include "../store_client.h"
#include "../cmd_protocol.h"
using namespace kvstore;
namespace test {
static void test_client_transaction() {
    std::thread t([&](){
        boost::asio::io_service ioservice2;
        ioservice2.run();
        StoreClient client(ioservice2);
        client.connect("127.0.0.1", "23333");
        StoreClient::Result result;
        result = client.use(2);
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);
        result = client.put("123", "456");
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);
        result = client.get("123");
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);
        result = client.mult();
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);
        result = client.put("333", "333");
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);
        result = client.put("444", "444");
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);
        result = client.put("555", "555");
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);
        result = client.exec();
        DEBUG_INFO<<"result:"<<result.second;
        result = client.get("333");
        EXCEPT_EQ(result.first, RESP_TYPE_OK);
        EXCEPT_EQ(result.second, "333");
        result = client.get("444");
        EXCEPT_EQ(result.first, RESP_TYPE_OK);
        EXCEPT_EQ(result.second, "444");
        result = client.get("555");
        EXCEPT_EQ(result.first, RESP_TYPE_OK);
        EXCEPT_EQ(result.second, "555");

       // client.del("123");
       // client.get("123");
    });
    t.detach();
}

static void test_client_transaction2() {
    std::thread t([&](){
        boost::asio::io_service ioservice2;
        ioservice2.run();
        StoreClient client(ioservice2);
        client.connect("127.0.0.1", "23333");
        StoreClient::Result result;
        result = client.use(2);
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);

        result = client.put("123", "456");
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);

        result = client.get("123");
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);

        result = client.mult();
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);

        result = client.mult();
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_ERR);

        result = client.use(3);
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_ERR);

        result = client.put("555", "555");
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);

        result = client.exec();
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);

        result = client.get("555");
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);
        EXCEPT_EQ(result.second, "555");

        result = client.del("555");
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);

        result = client.get("555");
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_ERR);
        EXCEPT_EQ(result.second, "null");

        result = client.del("555");
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_ERR);
        EXCEPT_EQ(result.second, "null");
       // client.del("123");
       // client.get("123");
    });
    t.detach();
}

static void test_client_transaction3() {
    std::thread t([&](){
        boost::asio::io_service ioservice2;
        ioservice2.run();
        StoreClient client(ioservice2);
        client.connect("127.0.0.1", "23333");
        StoreClient::Result result;
        result = client.use(2);
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);

        result = client.put("123", "456");
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);

        result = client.get("123");
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);

        result = client.mult();
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);

        result = client.put("999", "456");
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);

        result = client.exec();
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);

        result = client.mult();
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);

        result = client.discard();
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);

        result = client.discard();
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_ERR);

        result = client.watch("999");
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);

        result = client.put("999", "99999");
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);

        result = client.mult();
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);

        result = client.put("324", "432");
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);

        result = client.put("432", "522");
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_OK);

        result = client.exec();
        DEBUG_INFO<<"result:"<<result.second;
        EXCEPT_EQ(result.first, RESP_TYPE_ERR);

    });
    t.detach();
}

}
#endif // TEST_CLIENT

