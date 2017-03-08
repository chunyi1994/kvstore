#ifndef CONFIG_H
#define CONFIG_H
#include <string>
namespace kvstore {

class Config {
public:
    Config();
    Config(std::string path);
private:
    void init(std::string path);
    int parse(std::string path);
private:
};
//方便测试，把函数声明到头文件
int parse_line(const std::string &line, std::string &key, std::string &value);


} //namespace
#endif // CONFIG_H
