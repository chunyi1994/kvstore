#ifndef KVDATABASE_H
#define KVDATABASE_H

#include <unordered_map>
//#include <map>
#include <vector>
#include <string>

namespace kvstore {
class KVDatabase {
    typedef std::unordered_map<std::string, std::string> StringMap;
public:
    KVDatabase();
    KVDatabase(std::size_t num);

    std::size_t size() const;

    int clear(std::size_t id);
    int put(std::size_t id, std::string key, std::string value);
    int get(std::size_t id, std::string key, std::string& value) const;
    int del(std::size_t id, std::string key);

private:
    std::vector<StringMap> maps_;
};


} //namespace

#endif // KVDATABASE_H
