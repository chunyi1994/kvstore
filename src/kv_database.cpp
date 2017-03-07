#include "kv_database.h"
#include <assert.h>
namespace kvstore {

const int DEFAULT_DB_ID = 0;
const int DEFAULT_DB_NUM = 10;

KVDatabase::KVDatabase() :
    maps_(DEFAULT_DB_NUM),
    database_id_(DEFAULT_DB_ID)
{

}

KVDatabase::KVDatabase(std::size_t num) :
    maps_(num),
    database_id_(DEFAULT_DB_ID)
{

}

KVDatabase::KVDatabase(std::size_t num, std::size_t use_id) :
    maps_(num),
    database_id_(use_id)
{
    assert(num > use_id);
}

std::size_t KVDatabase::db_num() const {
    return maps_.size();
}

std::size_t KVDatabase::db_current_id() const {
    return database_id_;
}

int KVDatabase::use_db(std::size_t id) const {
    if (id >= db_num()) {
        return -1;
    }
    database_id_ = id;
    return 0;
}

int KVDatabase::put(std::string key, std::string value) {
    maps_[database_id_][key] = value;
    return 0;
}

int KVDatabase::get(std::string key, std::string &value) const {
    auto iter = maps_[database_id_].find(key);
    if (iter == maps_[database_id_].end()) {
        return -1;
    }
    value = iter->second;
    return 0;
}

int KVDatabase::del(std::string key)
{
    auto iter = maps_[database_id_].find(key);
    if (iter == maps_[database_id_].end()) {
        return -1;
    }
    maps_[database_id_].erase(iter);
    return 0;
}

}//namespace

