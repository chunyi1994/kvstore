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
    KVDatabase(std::size_t num, std::size_t use_id);

    std::size_t db_num() const;
    std::size_t db_current_id() const;
    int use_db(std::size_t id) const;
    int put(std::string key, std::string value);
    int get(std::string key, std::string& value) const;
    int del(std::string key);

private:
    std::vector<StringMap> maps_;
    mutable std::size_t database_id_;
};


} //namespace

#endif // KVDATABASE_H
