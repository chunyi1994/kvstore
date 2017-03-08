#ifndef CONN_DATA
#define CONN_DATA
#include <string>
#include "transaction.h"

namespace kvstore {

struct ConnData {
    ConnData() : db(0), buf(), transaction(), login(false) {}
    std::size_t db;
    std::string buf;
    Transaction transaction;
    bool login;
};
//buf高水位
}
#endif // CONN_DATA

