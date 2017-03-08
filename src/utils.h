#ifndef UTILS
#define UTILS
#include <sstream>
#include <string>
#include <time.h>
namespace utils{

template<class T>
static std::string to_string(const T& value) {
    std::stringstream ss;
    ss << value;
    std::string str;
    ss >> str;
    return str;
}

static std::string& trim(std::string &s) {
    if (s.empty()) {
        return s;
    }
    s.erase(0,s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

static std::string& trim_quo(std::string &s) {
    if (s.empty()) {
        return s;
    }
    s.erase(0,s.find_first_not_of("\""));
    s.erase(s.find_last_not_of("\"") + 1);
    return s;
}


}
#endif // UTILS

