#ifndef PERSISTENCE_H
#define PERSISTENCE_H
//#include "store_server.h"
#include "cmd_recoder.h"
#include "recovery.h"
namespace kvstore {
class StoreServer;

class Persistence
{
public:
    Persistence(StoreServer& server);
     int record(const std::string& cmd, int databaseid);
     int recover();

private:
     StoreServer& server_;
    CmdRecoder cmdRecoder_;
};

} //namepsace

#endif // PERSISTENCE_H
