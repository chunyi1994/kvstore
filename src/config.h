#ifndef CONFIG_H
#define CONFIG_H
#include <string>
namespace kvstore {

struct ConfigInfo {
    ConfigInfo() : need_password(false), persistence(false), password("") {}
    bool need_password;
    bool persistence;
    std::string password;
};

class Config {
public:
    Config();
    Config(const std::string& path);

    bool need_persistence() const { return info_.persistence; }
    bool need_password() const { return info_.need_password; }
    std::string password() const { return info_.password; }

private:
    void init(const std::string& path);
    int parse(const std::string& path);
    void process_password(const std::string& value);
    void process_persistence(const std::string& value);
private:
    ConfigInfo info_;
};
//方便测试，把函数声明到头文件
int parse_line(const std::string &line, std::string &key, std::string &value);


} //namespace
#endif // CONFIG_H
