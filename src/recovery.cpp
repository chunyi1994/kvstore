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
    std::vector<std::string> cmds;
    std::string filename = CMD_SAVE_PATH_FOLDER;
     int err = 0;
    for (std::size_t i = 0; i < server.db().size(); ++i) {
        filename = CMD_SAVE_PATH_FOLDER;
        filename = filename + "/db" + utils::to_string(i) + ".kvdb";
        cmds.clear();
        err = read_file(filename, cmds);
        if (err < 0) {
            LOG_DEBUG<<"read_file error,filename:" << filename;
            continue;
        }
        //执行命令
        err = execute_cmds(server, i, cmds);
        if (err < 0) {
            //命令有一条执行失败，就清空那一整个数据库
            LOG_DEBUG<<"cmd syntax error";
            server.db().clear(i);
            continue;
        }
    }
    return 0;
}

int Recovery::execute_cmds(StoreServer &server, std::size_t db,const std::vector<std::string> &cmds) {
    std::string msg;
    int err;
    for (std::size_t i = 0; i < cmds.size(); ++i) {
        err = server.execute_cmd(cmds[i], db, msg);
        if (err < 0) {
            LOG_ERROR<<"第"<<i<<"个db文件命令错误";
            return err;
        }
    }
    return err;
}

int Recovery::read_file(const std::string& filename, std::vector<std::string>& cmds) {
    std::ifstream ifs;
    std::string line;
    std::string cmdbuf;
    ifs.open(filename, std::ios::binary);
    if (!ifs) {
        LOG_DEBUG<<"file error";
        return -2;
    }
    while (!cmdbuf.empty() || std::getline(ifs, line)) {
        cmdbuf.append(line);
        line = "";
        if (cmdbuf.length() <= 3) {
            if (!ifs) {
                LOG_DEBUG<<"cmd syntax error";
                return -1;
            }
            continue;
        }
        unsigned short len = *((unsigned short*) (cmdbuf.data() + POS_TOTAL_LENGTH));
        len = boost::asio::detail::socket_ops::network_to_host_short(len);
        if (cmdbuf.length() < len) {
            if (!ifs) {
                LOG_DEBUG<<"cmd syntax error";
                return -1;
            }
            continue;
        }
        cmds.push_back( cmdbuf.substr(0, len));
        cmdbuf.erase(0, len);
    }
    return 0;
}


} //namespace
