#include "config.h"
#include <map>
#include <fstream>
#include "logging/logger.h"
#include "utils.h"
namespace kvstore {

static const char* DEFALUT_CONFIG_PATH = "config/config";


Config::Config() {
    init(DEFALUT_CONFIG_PATH);
}

Config::Config(std::string path) {
    init(path);
}

void Config::init(std::string path) {
    int err = parse(path);
}

int Config::parse(std::string path) {
    std::ifstream ifs(path);
    if (!ifs) {
        LOG_DEBUG<<"file error.";
        return -1;
    }
    std::map<std::string, std::string> configs;
    std::string line;
    std::string key;
    std::string value;
    while (std::getline(ifs, line)) {
        parse_line(line, key, value);
        configs[key] = value;
    }
    return 0;
}



int parse_line(const std::string &line, std::string &key, std::string &value) {
    std::size_t pos = 0;
    pos = line.find(":");
    if (pos == std::string::npos) {
        LOG_DEBUG<<"config parse error";
        return -1;
    }
    key = line.substr(0, pos);
    utils::trim(key);
    ++pos;
    if (pos >= line.length()) {
        LOG_DEBUG<<"config parse error: no value";
        return -2;
    }
    value = line.substr(pos, line.length() - pos);
    utils::trim(value);
    utils::trim_quo(value);
    if (value.empty()) {
        LOG_DEBUG<<"config parse error: no value";
        return -3;
    }
    return 0;
}
} //namespace
