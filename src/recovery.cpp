#include "recovery.h"
#include <vector>
#include <boost/asio.hpp>
#include <fstream>
#include "utils.h"
#include "cmd_protocol.h"
#include "logging/logger.h"
#include "store_server.h"
namespace kvstore {
Recovery::Recovery()
{
}

int Recovery::recover(StoreServer &server) {
    std::string cmdbuf("");
    std::string line("");
    std::vector<std::string> cmds;
    std::ifstream ifs;
    std::string filename = CMD_SAVE_PATH_FOLDER;
     int err = 0;
    for (std::size_t i = 0; i < server.db().db_num(); ++i) {
        filename = filename + "/db" + utils::to_string(i) + ".kvdb";
        ifs.open(filename, std::ios::binary);
        if (!ifs) { break; }
        while (!cmdbuf.empty() || std::getline(ifs, line)) {
            cmdbuf.append(line);
            line = "";
            if (cmdbuf.length() <= 3) {
                if (!ifs) return -1;
                continue;
            }
            unsigned short len = *((unsigned short*) (cmdbuf.data() + POS_TOTAL_LENGTH));
            len = boost::asio::detail::socket_ops::network_to_host_short(len);
            if (cmdbuf.length() < len) {
                if (!ifs) return -1;
                continue;
            }
            cmds.push_back( cmdbuf.substr(0, len));
            cmdbuf.erase(0, len);
        }
        err = execute_cmds(server, cmds);
        if (err < 0) { continue; }
    }
    return 0;
}

int Recovery::execute_cmds(StoreServer &server, const std::vector<std::string> &cmds) {
    std::string msg;
    int err;
    for (std::size_t i = 0; i < cmds.size(); ++i) {
        err = server.execute_cmd(cmds[i], msg);
        if (err < 0) {
            LOG_ERROR<<"第"<<i<<"个db文件命令错误";
            return err;
        }
    }
    return 0;
}

int Recovery::read_file(StoreServer &server) {


}


} //namespace
