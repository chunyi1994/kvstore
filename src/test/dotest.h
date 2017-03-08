#include <thread>
#include "test.h"
#include "test/test_config.h"
#include "../store_server.h"
#include "../store_client.h"
#include "test/test_client.h"
using namespace kvstore;
namespace test {

static void do_test(boost::asio::io_service& ioservice) {
   test_config();
   test_client_transaction3();
   //test_client_transaction2();
   //test_client_transaction();
}

}//namespace
