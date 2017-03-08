#include "cmd_recoder.h"
#include <sys/stat.h>
#include <sys/types.h> //for mkdir
#include <sstream>
#include <assert.h>
#include<stdio.h> //rename
#include <logging/logger.h>
#include "utils.h"
#include "cmd_protocol.h"
namespace kvstore {


const int DEFAULT_DB_NUM = 10;

CmdRecoder::CmdRecoder(std::size_t dbnum) :
    dbnum_(dbnum),
    dbfiles_(dbnum)
{
    init();
}

CmdRecoder::CmdRecoder() :
    dbnum_(DEFAULT_DB_NUM),
    dbfiles_(DEFAULT_DB_NUM)
{
    init();
}

int CmdRecoder::record(const std::string &cmd, int databaseid) {
    int result = -1;
    assert(static_cast<std::size_t>(databaseid) <dbnum_ );
    if (static_cast<std::size_t>(databaseid) >= dbnum_) {
        return result;
    }
    int req_type = static_cast<int>(cmd[POS_TYPE]);
    switch (req_type) {
    case REQ_TYPE_PUT :
    case REQ_TYPE_DEL:
        dbfiles_[databaseid]<<cmd;
        dbfiles_[databaseid].flush();
        result = 1;
    default:
        result = 0;
    }
    return result;
}



int CmdRecoder::init() {
    int err = mkdir(CMD_SAVE_PATH_FOLDER ,
                         S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
    if (err != 0) {
        //LOG_INFO<<"文件夹创建失败";
        //return isCreate;
    }

    //创建文件
    std::string filename;
    for (int i = 0; i < static_cast<int>(dbnum_); ++i) {
        filename = CMD_SAVE_PATH_FOLDER;
        filename = filename + "/db" + utils::to_string(i) + ".kvdb";
        dbfiles_[i].open(filename, std::ios::app |  std::ios::binary);
        if (!dbfiles_[i]) {
            LOG_DEBUG<<"第"<<i<<"个文件创建失败";
            continue;
        }
    }

    return err;
}

} //namespace

