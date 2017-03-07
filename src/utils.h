#ifndef UTILS
#define UTILS
#include <sstream>
#include <string>
namespace utils{

template<class T>
static std::string to_string(const T& value) {
    std::stringstream ss;
    ss << value;
    std::string str;
    ss >> str;
    return str;
}

}
#endif // UTILS

