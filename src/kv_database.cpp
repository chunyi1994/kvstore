#include "kv_database.h"
#include <assert.h>
#include "logging/logger.h"
namespace kvstore {

//const int DEFAULT_DB_ID = 0;
const int DEFAULT_DB_NUM = 10;

KVDatabase::KVDatabase() :
    maps_(DEFAULT_DB_NUM)
{
}

KVDatabase::KVDatabase(std::size_t num) :
    maps_(num)
{

}

std::size_t KVDatabase::size() const {
    return maps_.size();
}

int KVDatabase::clear(std::size_t id) {
    if (id >= size()) {
        LOG_DEBUG<<"err";
        return -1;
    }
    maps_[id].clear();
    return 0;
}

int KVDatabase::put(std::size_t id, std::string key, std::string value) {
    if (id >= size()) {
        LOG_DEBUG<<"err";
        return -1;
    }
    maps_[id][key] = value;
    return 0;
}

int KVDatabase::get(std::size_t id, std::string key, std::string &value) const {
    if (id >= size()) {
        LOG_DEBUG<<"err";
        return -2;
    }
    auto iter = maps_[id].find(key);
    if (iter == maps_[id].end()) {
        LOG_DEBUG<<"err";
        return -1;
    }
    value = iter->second;
    return 0;
}

int KVDatabase::del(std::size_t id, std::string key) {
    if (id >= size()) {
        LOG_DEBUG<<"err";
        return -2;
    }
    auto iter = maps_[id].find(key);
    if (iter == maps_[id].end()) {
        LOG_DEBUG<<"err";
        return -1;
    }
    maps_[id].erase(iter);
    return 0;
}

}//namespace

