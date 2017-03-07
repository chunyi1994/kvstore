#ifndef CMDRECODER_H
#define CMDRECODER_H
#include <string>
#include <vector>
#include <fstream>
namespace kvstore {

class CmdRecoder
{
public:
    CmdRecoder(std::size_t dbnum);
    CmdRecoder();
    int record(const std::string& cmd, int databaseid);
private:
    int init();
private:
    std::size_t dbnum_;
    std::vector<std::ofstream> dbfiles_;
};
static const char* CMD_SAVE_PATH_FOLDER = "cmd_save/";

}
#endif // CMDRECODER_H
