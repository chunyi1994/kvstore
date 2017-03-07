#include "cmd_recoder.h"
#include <sys/stat.h>
#include <sys/types.h> //for mkdir
#include <sstream>
#include <assert.h>
#include<stdio.h> //rename
#include <logging/logger.h>
#include "utils.h"

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
    assert(static_cast<std::size_t>(databaseid) <dbnum_ );
    if (static_cast<std::size_t>(databaseid) >= dbnum_) {
        return -1;
    }
    dbfiles_[databaseid]<<cmd;
    //LOG_DEBUG<<"写入："<<cmd;
    dbfiles_[databaseid].flush();
    return 0;
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

