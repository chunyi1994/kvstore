#ifndef RECOVERY_H
#define RECOVERY_H
#include "kv_database.h"
#include <vector>
namespace kvstore {
class StoreServer;
class Recovery {
public:
    Recovery();
    int recover(StoreServer &server);

private:
    int execute_cmds(StoreServer &server, const std::vector<std::string>& cmds);
    int read_file(StoreServer &server);
private:

};

} //namespace

#endif // RECOVERY_H
