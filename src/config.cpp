#include "config.h"
#include <map>
#include <fstream>
#include "logging/logger.h"
#include "utils.h"
namespace kvstore {

static const char* DEFALUT_CONFIG_PATH = "config/config";


Config::Config() : info_() {
    init(DEFALUT_CONFIG_PATH);
}

Config::Config(const std::string& path) : info_() {
    init(path);
}

void Config::init(const std::string& path) {
    int err = parse(path);
}

int Config::parse(const std::string& path) {
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

    for (auto &pair : configs) {
        if (pair.first == "password") {
            LOG_INFO<<"启用密码保护，密码为" <<pair.second;
            process_password(pair.second);
        } else if (pair.first == "persistence") {
            process_persistence(pair.second);
        }
    }
    return 0;
}


void Config::process_persistence(const std::string &value) {
    if (value == "on") {
        info_.persistence = true;
        LOG_INFO<<"启用持久化";
    } else {
        info_.persistence = false;
    }
}

void Config::process_password(const std::string &value) {
    info_.need_password = true;
    info_.password = value;
}

int parse_line(const std::string &line, std::string &key, std::string &value) {
    std::size_t pos = 0;
    pos = line.find(":");
    if (pos == std::string::npos) {
        LOG_DEBUG<<"config parse error, line:" <<line;
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
