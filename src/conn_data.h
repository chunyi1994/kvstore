#ifndef CONN_DATA
#define CONN_DATA
namespace kvstore {
#include <string>
struct ConnData {
    ConnData() : db(0), buf() {}
    std::size_t db;
    std::string buf;
};

}
#endif // CONN_DATA

