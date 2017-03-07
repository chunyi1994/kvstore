#include "persistence.h"

#include "cmd_protocol.h"
namespace kvstore {
Persistence::Persistence(StoreServer& server):
    server_(server),
    cmdRecoder_()
{

}

int Persistence::record(const std::string &cmd, int databaseid) {
    return cmdRecoder_.record(cmd, databaseid);
}

int Persistence::recover() {
    Recovery recovery;
    return recovery.recover(server_);
}


} //namespace

