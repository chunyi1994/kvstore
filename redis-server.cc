/*************************************************************************
  > File Name: redis-server.cc
  > Author: Sunfish
  > Mail: 704613309@qq.com
  > Created Time: 2016年07月27日 星期三 09时39分39秒
 ************************************************************************/
#include "utils.h"
#include "redis-server.h"
#include <vector>
#include <sstream>
#include <cstdlib>
#include <assert.h>
#include <iterator>
#include <algorithm>
#include <iterator>
namespace libevent{

const char * ERROR_SYNAX = "Error: Synax wrong.\n";
const char * ERROR_TYPE = "Error: Type wrong.\n";
const char * RESULT_1 = "(interger)1\n";
const char * RESULT_0 = "(interger)0\n";
const char * RESULT_OK = "OK\n";
const char * RESULT_NULL = "NULL\n";
const char * RESULT_UNKNOW = "Error: Unknow\n";

const int LEFT_PUSH = 1;
const int RIGHT_PUSH = 2;
const int LEFT_POP = 1;
const int RIGHT_POP = 2;

const int OP_DIFF = 1;
const int OP_UNION = 2;
const int OP_INTER  = 3;


void RedisServer::readCallback(bufferevent* bev, const std::string& msg){

    std::vector<std::string> vec;
    std::istringstream is(msg);
    std::string line;
    while(std::getline(is, line)){
        log(line);
        vec.push_back(line);
    }
    processMsg(bev, vec);
}

void RedisServer::connectionCallback(bufferevent *bev, short event){
    if(event == EVENT_CLOSE){
        log("connection close.");
    }else if(event == EVENT_NEW_CONNECTION){
        log("new connection.");
    }
}

void RedisServer::processMsg(bufferevent* bev, std::vector<std::string>& vec){

    for(size_t i = 0; i < vec.size(); i++){
        std::istringstream is(vec[i]);
        std::string instruct;
        is>>instruct;
        //不区分大小写的比较
        if(compareNoCase(instruct, "get")){
            cmdGet(bev, vec[i]);
        }else if(compareNoCase(instruct, "set")){
            cmdSet(bev, vec[i]);
        }else if(compareNoCase(instruct, "incrby")){
            cmdIncrby(bev, vec[i]);
        }else if(compareNoCase(instruct, "exists")){
            cmdExists(bev, vec[i]);
        }else if(compareNoCase(instruct, "hget")){
            cmdHget(bev, vec[i]);
        }else if(compareNoCase(instruct, "hset")){
            cmdHset(bev, vec[i]);
        }else if(compareNoCase(instruct, "hexists")){
            cmdHexists(bev, vec[i]);
        }else if(compareNoCase(instruct, "hgetall")){
            cmdHgetall(bev, vec[i]);
        }else if(compareNoCase(instruct, "del")){
            cmdDel(bev, vec[i]);
        }else if(compareNoCase(instruct, "hdel")){
            cmdHdel(bev, vec[i]);
        }else if(compareNoCase(instruct, "hincrby")){
            cmdHincrby(bev, vec[i]);
        }else if(compareNoCase(instruct, "lpush")){
            cmdListPush(bev, vec[i], LEFT_PUSH);
        }else if(compareNoCase(instruct, "rpush")){
            cmdListPush(bev, vec[i], RIGHT_PUSH);
        }else if(compareNoCase(instruct, "lrange")){
            cmdLrange(bev, vec[i]);
        }else if(compareNoCase(instruct, "lrem")){
            cmdLrem(bev, vec[i]);
        }else if(compareNoCase(instruct, "lindex")){
            cmdLindex(bev, vec[i]);
        }else if(compareNoCase(instruct, "lpop")){
            cmdListPop(bev, vec[i], LEFT_POP);
        }else if(compareNoCase(instruct, "rpop")){
            cmdListPop(bev, vec[i], RIGHT_POP);
        }else if(compareNoCase(instruct, "linsert")){
            cmdLinsert(bev, vec[i]);
        }else if(compareNoCase(instruct, "sadd")){
            cmdSadd(bev, vec[i]);
        }else if(compareNoCase(instruct, "type")){
            cmdType(bev, vec[i]);
        }else if(compareNoCase(instruct, "srem")){
            cmdSrem(bev, vec[i]);
        }else if(compareNoCase(instruct, "sismember")){
            cmdSismember(bev, vec[i]);
        }else if(compareNoCase(instruct, "smembers")){
            cmdSmembers(bev, vec[i]);
        }else if(compareNoCase(instruct, "sdiff")){
            cmdSOp(bev, vec[i], OP_DIFF);
        }else if(compareNoCase(instruct, "sinter")){
            cmdSOp(bev, vec[i], OP_INTER);
        }else if(compareNoCase(instruct, "sunion")){
            cmdSOp(bev, vec[i], OP_UNION);
        }else{
            send(bev, RESULT_UNKNOW);
        }


    }
}
void RedisServer::startCallback(){
    log("Tiny Redis Server is started!");
}
void RedisServer::cmdGet(bufferevent *bev, const std::string& msg){
    std::istringstream is(msg);
    std::string key;
    std::string value;
    std::string instruct;
    is>>instruct;
    is>>key;
    //语法检查
    if(key == ""){
        send(bev, ERROR_SYNAX);
        return;
    }
    if(mapExceptTypeFind(key, typeid(StringData))){
        value = ERROR_TYPE;
    }
    //查找是否存在
    if(mapFindString(key)){
        value = mapGetString(key);
        value = "\"" + value + "\"\n";
    }else{
        value = RESULT_NULL;
    }
    send(bev, value);
}

void RedisServer::cmdDel(bufferevent *bev, const std::string& msg){
    std::istringstream is(msg);
    std::string key;
    std::string instruct;
    is>>instruct;
    is>>key;
    //语法检查
    if(key == ""){
        send(bev, ERROR_SYNAX);
        return;
    }
    /*
    if(mapExceptTypeFind(key, typeid(StringData))){
        value = ERROR_TYPE;
    }*/
    if(!mapExists(key)){
         send(bev, ERROR_TYPE);
         return;
    }

    //删除
    if(mapRemove(key)){
        send(bev, RESULT_1);
    }else{
        send(bev, RESULT_0);
    }
}

void RedisServer::cmdHexists(bufferevent *bev, const std::string& msg){
    std::istringstream is(msg);
    std::string field;
    std::string key;
    std::string instruct;
    is>>instruct;
    is>>field;
    is>>key;
    //语法检查
    if(field == "" || key == ""){
        send(bev, ERROR_SYNAX);
        return;
    }
    //是否存在别的类型的key占用了这个field
    if(mapExceptTypeFind(key, typeid(MapData))){
        send(bev, ERROR_TYPE);
        return;
    }

    if(!mapFindMapSecond(field, key)){
        send(bev, RESULT_0);
    }else{
        send(bev, RESULT_1);
    }
}

void RedisServer::cmdExists(bufferevent* bev, const std::string& msg){
    std::istringstream is(msg);
    std::string str;
    std::string instruct;
    is>>instruct;
    is>>str;
    //语法检查
    if(str == ""){
        send(bev, ERROR_SYNAX);
        return;
    }
    //检查是否存在
    if(mapExists(str)){
        send(bev, RESULT_1);
    }else{
        send(bev, RESULT_0);
    }
}

void RedisServer::cmdSet(bufferevent *bev, const std::string& msg){
    std::istringstream is(msg);
    std::string key;
    std::string value;
    std::string instruct;
    is>>instruct;
    is>>key;
    is>>value;
    //语法检查
    if(key == "" || value == ""){
        send(bev,ERROR_SYNAX);
        return;
    }

    //如果有不是string的类型已经存在,则返回
    if(mapExceptTypeFind(key, typeid(StringData))){
        send(bev, ERROR_TYPE);
        return ;
    }

    mapSetString(key, value);
    send(bev, RESULT_OK);
}

void RedisServer::cmdHset(bufferevent *bev, const std::string& msg){
    std::istringstream is(msg);
    std::string field;
    std::string key;
    std::string value;
    std::string instruct;
    is>>instruct;
    is>>field;
    is>>key;
    is>>value;
    //检查语法
    if(key == "" || field == "" || value == ""){
        send(bev,ERROR_SYNAX);
        return;
    }
    if(mapExceptTypeFind(field, typeid(MapData))){
        send(bev, ERROR_TYPE);
        return;
    }
    //查找到目标,并赋值,如果类型不匹配,则返回错误
    int retNum = mapSetMap(field, key, value);
    std::string ret = int2string(retNum);
    //stringChange(ret);
    ret = "(interger)" + ret + "\n";
    send(bev, ret);
}


void RedisServer::cmdHgetall(bufferevent *bev, const std::string& msg){
    std::istringstream is(msg);
    std::string field;
    std::string instruct;
    is>>instruct;
    is>>field;
    if(field == ""){
        send(bev, ERROR_SYNAX);
        return;
    }

    if(mapExceptTypeFind(field, typeid(MapData))){
        send(bev, ERROR_TYPE);
        return;
    }
    std::string ret;
    //取得map,然后格式化成string
    if(mapFindMap(field)){
        ret = mapGetAllMapString(field);
    }else{
        //不存在则返回错误
        ret = RESULT_NULL;
    }
    send(bev, ret);
}

void RedisServer::cmdHdel(bufferevent *bev, const std::string& msg){
    std::istringstream is(msg);
    std::string field;
    std::string key;
    std::string value;
    std::string instruct;
    is>>instruct;
    is>>field;
    is>>key;
    if(field == "" || key == ""){
        send(bev, ERROR_SYNAX);
        return;
    }

    if(mapExceptTypeFind(field, typeid(MapData))){
        send(bev, ERROR_TYPE);
        return;
    }

    //查找,再删除
    DataPtr data = mapFind(field);
    if(data == NULL || typeid(*data) != typeid(MapData)){
        send(bev, RESULT_0);
    }else{
        mapMapRemove(field, key);
        send(bev, RESULT_1);
    }

}

void RedisServer::cmdHget(bufferevent *bev, const std::string& msg){
    std::istringstream is(msg);
    std::string field;
    std::string key;
    std::string value;
    std::string instruct;
    is>>instruct;
    is>>field;
    is>>key;
    if(field == "" || key == ""){
        send(bev, ERROR_SYNAX);
        return;
    }

    if(mapExceptTypeFind(field, typeid(MapData))){
        send(bev, ERROR_TYPE);
        return;
    }

    if(mapFindMap(field)){
        value = mapGetMapSecond(field, key);
        value = "\"" + value + "\"\n";
    }else{
        value = RESULT_NULL;
    }
    send(bev, value);
}

void RedisServer::cmdHincrby(bufferevent *bev, const std::string& msg){
    std::istringstream is(msg);
    std::string field;
    std::string key;
    std::string instruct;
    std::string incr;
    is>>instruct;
    is>>field;
    is>>key;
    is>>incr;
    //检查语法
    if(field == "" || key == ""){
        send(bev, ERROR_SYNAX);
        return;
    }

    if(mapExceptTypeFind(field, typeid(MapData))){
        send(bev, ERROR_TYPE);
        return;
    }

    //检查是否存在
    if(!mapFindMap(field)){
        send(bev, RESULT_NULL);
        return;
    }

    //取得元素
    //检查是否是数字
    if(!isDigit(incr)){
        send(bev, ERROR_TYPE);
        return;
    }

    std::string value = mapMapStringIncr(field, key, incr);
    if(value != "no"){
        send(bev, "(interger)"+value+"\n");
    }else{
        send(bev, ERROR_TYPE);
    }

}
//terrible design
void RedisServer::cmdIncrby(bufferevent *bev, const std::string& msg){
    std::istringstream ss(msg);
    std::string key;
    std::string incr;
    std::string instruct;
    ss>>instruct;
    ss>>key;
    ss>>incr;

    if(key == "" || incr == ""){
        send(bev,ERROR_SYNAX);
        return;
    }

    if(mapExceptTypeFind(key, typeid(StringData))){
        send(bev, ERROR_TYPE);
        return;
    }
    //检查是否是数字
    if(!isDigit(incr)){
        send(bev, ERROR_SYNAX);
        return;
    }
    std::string value = mapStringIncr(key, incr);
    if(value != "no"){
        send(bev, "(interger)"+value+"\n");
    }else{
        send(bev, RESULT_NULL);
    }

}

void RedisServer::cmdType(bufferevent *bev, const std::__cxx11::string &msg)
{
    std::istringstream is(msg);
    std::string key;
    std::string instruct;
    is>>instruct;
    is>>key;
    std::string ret = mapGetType(key);
    ret = ret + "\n";
    send(bev, ret);
}

void RedisServer::cmdListPush(bufferevent *bev, const std::string& msg, int pushDirection){
    std::istringstream is(msg);
    std::string key;
    std::string value;
    std::string instruct;
    is>>instruct;
    is>>key;
    is>>value;
    //语法检查
    if(key == "" || value == ""){
        send(bev,ERROR_SYNAX);
        return;
    }

    if(mapExceptTypeFind(key, typeid(ListData))){
        send(bev, ERROR_TYPE);
        return;
    }

    //这里需要改进,非线程安全
    //查找到目标,并赋值,如果类型不匹配,则返回错误
    DataPtr data = mapFind(key);
    if(data == NULL || typeid(*data) == typeid(ListData)){
        int num = mapListPush(key, value, pushDirection);

        std::string ret = int2string(num);
        stringChange(ret);
        send(bev, ret);
    }else{
        send(bev, ERROR_TYPE);
    }
}

void RedisServer::cmdLinsert(bufferevent *bev, const std::string& msg){
    std::istringstream is(msg);
    std::string key;
    std::string beforeOrAfter;
    std::string targetValue;
    std::string insertValue;
    std::string instruct;
    is>>instruct;
    is>>key;
    is>>beforeOrAfter;
    is>>targetValue;
    is>>insertValue;
    //语法检查
    if(key == "" || beforeOrAfter == "" || targetValue == "" || insertValue == ""
            ||! (compareNoCase(beforeOrAfter,"before") || compareNoCase(beforeOrAfter, "after"))){
        send(bev,ERROR_SYNAX);
        return;
    }

    //确保存在一个key的list
    if(!mapFindList(key)){
        send(bev, ERROR_TYPE);
        return;
    }
    int position;
    if(compareNoCase(beforeOrAfter, "before")){
        position = LEFT_PUSH;
    }else{
        position = RIGHT_PUSH;
    }
    int ret = mapListInsert(key, targetValue, insertValue, position);
    if(ret != -1){
        send(bev, int2string(ret));
    }else{
        send(bev, RESULT_NULL);
    }
}




void RedisServer::cmdListPop(bufferevent *bev, const std::string& msg, int popDirection){
    std::istringstream is(msg);
    std::string key;
    std::string instruct;
    is>>instruct;
    is>>key;
    //语法检查
    if(key == ""){
        send(bev,ERROR_SYNAX);
        return;
    }

    //确保存在一个key的list
    if(!mapFindList(key)){
        send(bev, ERROR_TYPE);
        return;
    }

    std::string ret= mapListPop(key, popDirection);
    stringChange(ret);
    send(bev, ret);
}

void RedisServer::cmdLrange(bufferevent *bev, const std::string& msg){
    std::istringstream ss(msg);
    std::string key;
    std::string left;
    std::string right;
    std::string instruct;
    ss>>instruct;
    ss>>key;
    ss>>left;
    ss>>right;

    if(key == "" || right == "" || left == ""){
        send(bev,ERROR_SYNAX);
        return;
    }

    //确保存在一个key的list
    if(!mapFindList(key)){
        send(bev, ERROR_TYPE);
        return;
    }

    //检查是否是数字
    if(!isDigit(right) || !isDigit(left)){
        send(bev, ERROR_TYPE);
        return;
    }

    int leftInt = string2int(left);
    int rightInt = string2int(right);
    std::string value = mapListRange(key, leftInt, rightInt);
    //返回
    send(bev, value);
}

void RedisServer::cmdLrem(bufferevent *bev, const std::string& msg){
    std::istringstream ss(msg);
    std::string key;
    std::string count;
    std::string value;
    std::string instruct;
    ss>>instruct;
    ss>>key;
    ss>>count;
    ss>>value;

    if(key == "" || count == "" || value == ""){
        send(bev,ERROR_SYNAX);
        return;
    }

    //确保存在一个key的list
    if(!mapFindList(key)){
        send(bev, ERROR_TYPE);
        return;
    }

    //检查是否是数字
    if(!isDigit(count) ){
        send(bev, ERROR_TYPE);
        return;
    }

    int countInt = string2int(count);
    int ret = mapListRemove(key, countInt, value);

    if(ret == -1){
        send(bev, ERROR_TYPE);
    }else if(ret == 0){
        send(bev, RESULT_0);
    }else{
        send(bev,int2string(ret) + "\n");
    }
    //返回
}

void RedisServer::cmdLindex(bufferevent *bev, const std::string& msg){
    std::istringstream ss(msg);
    std::string key;
    std::string index;
    std::string instruct;
    ss>>instruct;
    ss>>key;
    ss>>index;

    if(key == "" || index == ""){
        send(bev,ERROR_SYNAX);
        return;
    }

    //确保存在一个key的list
    if(!mapFindList(key)){
        send(bev, ERROR_TYPE);
        return;
    }

    //检查是否是数字
    if(!isDigit(index) ){
        send(bev, ERROR_TYPE);
        return;
    }

    int indexInt = string2int(index);
    std::string ret = mapListIndex(key, indexInt);

    send(bev, ret);

    //返回
}


void RedisServer::cmdSadd(bufferevent* bev, const std::string &msg){
    std::istringstream ss(msg);
    std::string key;
    std::string value;
    std::string instruct;
    ss>>instruct;
    ss>>key;
    ss>>value;

    if(key == "" || value == ""){
        send(bev,ERROR_SYNAX);
        return;
    }

    //确保不存在一个key对应其他类型
    if(mapExceptTypeFind(key, typeid(SetData))){
        send(bev, ERROR_TYPE);
        return;
    }

    int ret = mapSetSet(key,value);
    if(ret == 1){
        send(bev, RESULT_1);
    }else{
        send(bev, RESULT_0);
    }

}

void RedisServer::cmdSismember(bufferevent *bev, const std::string &msg){
    std::istringstream ss(msg);
    std::string key;
    std::string value;
    std::string instruct;
    ss>>instruct;
    ss>>key;
    ss>>value;

    if(key == "" || value == ""){
        send(bev,ERROR_SYNAX);
        return;
    }

    if(!mapFindSet(key)){
        send(bev, ERROR_TYPE);
        return;
    }

    bool ret = mapSetIsMember(key,value);
    if(ret){
        send(bev, RESULT_1);
    }else{
        send(bev, RESULT_0);
    }
}

void RedisServer::cmdSmembers(bufferevent *bev, const std::__cxx11::string &msg) {
    std::istringstream ss(msg);
    std::string key;
    std::string instruct;
    ss>>instruct;
    ss>>key;

    if(key == ""){
        send(bev,ERROR_SYNAX);
        return;
    }

    //确保key存在
    if(!mapFindSet(key)){
        send(bev, ERROR_TYPE);
        return;
    }

    std::string ret = mapSetAllMembers(key);
    send(bev, ret);
}

void RedisServer::cmdSOp(bufferevent *bev, const std::string &msg, int op)
{ std::istringstream ss(msg);
    std::string key1;
    std::string key2;
    std::string instruct;
    ss>>instruct;
    ss>>key1;
    ss>>key2;

    if(key1 == "" || key2 == ""){
        send(bev,ERROR_SYNAX);
        return;
    }

    //确保不存在一个key对应其他类型
    if(!mapFindSet(key1)  || !mapFindSet(key2)){
        send(bev, ERROR_TYPE);
        return;
    }

    std::string  ret  = mapSetOp(key1, key2, op);
    send(bev, ret);
}

void RedisServer::cmdSrem(bufferevent *bev, const std::string &msg){
    std::istringstream ss(msg);
    std::string key;
    std::string value;
    std::string instruct;
    ss>>instruct;
    ss>>key;
    ss>>value;

    if(key == "" || value == ""){
        send(bev,ERROR_SYNAX);
        return;
    }

    //确保存在一个key对应set
    if(!mapFindSet(key)){
        send(bev, ERROR_TYPE);
        return;
    }

    bool ret = mapSetRemove(key,value);
    if(ret){
        send(bev, RESULT_1);
    }else{
        send(bev, RESULT_0);
    }
}

//以下函数都含有assert炸弹
//使用前应该先检查参数
//加锁
//从map中设置键值对string
void RedisServer::mapSetString(const std::string& key, const std::string& value){
    std::lock_guard<std::mutex> lock(mutex_);
    DataPtr data = dbmap_[key];
    if(data == NULL){
        data = new StringData();
        dbmap_[key] = data;
    }
    assert(typeid(*data) == typeid(StringData));
    StringData* sData = dynamic_cast<StringData*>(data);
    sData->data_ = value;
    dbmap_[key] = sData;
}

//加锁
std::string& RedisServer::mapGetString(const std::string& key){
    std::lock_guard<std::mutex> lock(mutex_);
    DataPtr data = dbmap_[key];
    assert(data != NULL);
    assert(typeid(*data) == typeid(StringData));
    StringData* sData = dynamic_cast<StringData*>(data);
    return sData->data_;
}

std::string RedisServer::mapStringIncr(const std::string &key, const std::string& incr)
{
    std::lock_guard<std::mutex> lock(mutex_);
    DataPtr data = dbmap_[key];
    //assert(data != NULL);
    if(!data){
        data = new StringData();
    }
    assert(typeid(*data) == typeid(StringData));
    StringData* sData = dynamic_cast<StringData*>(data);
    if(!isDigit(sData->data_) && sData->data_.length() !=0 ){
        return "no";
    }
    int val = string2int( sData->data_) + string2int(incr);
    sData->data_ = int2string(val);
    dbmap_[key]  = sData;
    return  sData->data_;
}

//加锁
//存在key返回true
DataPtr RedisServer::mapFind(const std::string &key){
    std::lock_guard<std::mutex> lock(mutex_);
    DBMap::iterator iter = dbmap_.find(key);
    if(iter == dbmap_.end()){
        return NULL;
    }
    DataPtr data = iter->second;
    return data;
}
//加锁
//存在和预想中不一样的类型的key返回true
bool RedisServer::mapExceptTypeFind(const std::string &key, const std::type_info& info){
    std::lock_guard<std::mutex> lock(mutex_);
    DBMap::iterator iter = dbmap_.find(key);
    if(iter == dbmap_.end()){
        return false;
    }

    DataPtr data = iter->second;
    if(typeid(*data) != info){
        return true;
    }
    return false;
}
//加锁
//存在所需要的类型的值返回true
bool RedisServer::mapFind(const std::string &key, const std::type_info& info){
    std::lock_guard<std::mutex> lock(mutex_);
    DBMap::iterator iter = dbmap_.find(key);
    if(iter == dbmap_.end()){
        return false;
    }

    DataPtr data = iter->second;
    if(typeid(*data) == info){
        return true;
    }
    return false;
}

bool RedisServer::mapFindList(const std::string &key){
    return mapFind(key, typeid(ListData));
}

bool RedisServer::mapFindMapSecond(const std::string& field,const std::string& key){
    std::lock_guard<std::mutex> lock(mutex_);
    DBMap::iterator iter = dbmap_.find(field);
    if(iter == dbmap_.end()){
        return false;
    }

    DataPtr data = iter->second;
    if(data == NULL){
        return false;
    }
    MapData *mData = dynamic_cast<MapData*> (data);
    if(mData->map_.find(key) == mData->map_.end()){
        return false;
    }
    return true;

}

bool RedisServer::mapFindSet(const std::string& key){
    return mapFind(key, typeid(SetData));
}

//不用加锁,因为mapFind已经有了
bool RedisServer::mapFindString(const std::string& key){
    return mapFind(key, typeid(StringData));
}

//不用加锁
bool RedisServer::mapFindMap(const std::string& key){
    return mapFind(key, typeid(MapData));
}

//加锁
bool RedisServer::mapExists(const std::string& key){
    std::lock_guard<std::mutex> lock(mutex_);
    DBMap::iterator iter = dbmap_.find(key);
    if(iter == dbmap_.end()){
        return false;
    }
    return true;
}

//加锁
//从map中取得value,这个value也是map
Map& RedisServer::mapGetMap(const std::string& key){
    std::lock_guard<std::mutex> lock(mutex_);
    DataPtr data = dbmap_[key];
    assert(data != NULL);
    assert(typeid(*data) == typeid(MapData));
    MapData *mData = dynamic_cast<MapData*> (data);
    return mData->map_;
}

//加锁
//从map中取得value,这个value也是map
std::string RedisServer::mapGetAllMapString(const std::string& field){
    std::lock_guard<std::mutex> lock(mutex_);
    std::string key;
    std::string value;
    std::string ret;
    //std::string ret = "----------------\n";
    DataPtr data = dbmap_[field];
    assert(data != NULL);
    assert(typeid(*data) == typeid(MapData));
    MapData *mData = dynamic_cast<MapData*> (data);
    Map& refMap = mData->map_;
    if(refMap.size() == 0){
        return RESULT_NULL;
    }

    for(Map::iterator iter = refMap.begin();
        iter != refMap.end(); iter++){
        key = iter->first;
        value = iter->second;
        ret = ret +"\"" + key + "\"\n";
        ret = ret +"\"" + value + "\"\n";
      //  ret = ret + "----------------\n";
    }
    return ret;
}

std::string RedisServer::mapMapStringIncr(const std::string &field,
                                          const std::string &key, const std::string &incr) {
    std::lock_guard<std::mutex> lock(mutex_);
    DataPtr data = dbmap_[field];
    assert(data);
    assert(typeid(*data) == typeid(MapData));
    MapData *mData = dynamic_cast<MapData*> (data);
    if(!isDigit(mData->map_[key]) && mData->map_[key].length() != 0){
        return "no";
    }
    int value = string2int(mData->map_[key]) + string2int(incr);
    mData->map_[key] = int2string(value);
    return  mData->map_[key];
}

std::string&  RedisServer::mapGetMapSecond(const std::string & field, const std::string & key){
    std::lock_guard<std::mutex> lock(mutex_);
    DataPtr data = dbmap_[field];
    assert(data != NULL);
    assert(typeid(*data) == typeid(MapData));
    MapData *mData = dynamic_cast<MapData*> (data);
    return mData->map_[key];
}


//加锁
int RedisServer::mapSetMap(const std::string &field,
                           const std::string &key, const std::string &value){
    std::lock_guard<std::mutex> lock(mutex_);
    DataPtr data = dbmap_[field];
    if(data == NULL){
        data = new MapData();
        dbmap_[field] = data;
    }
    assert(typeid(*data) == typeid(MapData));
    MapData* sData = dynamic_cast<MapData*>(data);
    sData->map_[key] = value;
    dbmap_[field]=sData;
    return 1;
}

//加锁
bool RedisServer::mapRemove(const std::string& key){
    std::lock_guard<std::mutex> lock(mutex_);
    DBMap::iterator iter = dbmap_.find(key);
    if(iter == dbmap_.end()){
        return false;
    }else{
        DataPtr data = iter->second;
        dbmap_.erase(iter);
        delete data;
        return true;
    }
}
//加锁
bool RedisServer::mapMapRemove(const std::string &field, const std::string& key){
    std::lock_guard<std::mutex> lock(mutex_);
    DBMap::iterator iter = dbmap_.find(field);
    if(iter == dbmap_.end()){
        return false;
    }else{

        DataPtr data = iter->second;
        MapData* mData = dynamic_cast<MapData*>(data);
        Map::iterator it = mData->map_.find(key);
        if(it == mData->map_.end()){
            return false;
        }
        mData->map_.erase(it);
        return true;
    }
}

//加锁
int RedisServer::mapListPush(const std::string &key, const std::string& value, int pushDirection){
    std::lock_guard<std::mutex> lock(mutex_);
    DataPtr data = dbmap_[key];
    if(data == NULL){
        data = new ListData();
        dbmap_[key] = data;
    }
    assert(typeid(*data) == typeid(ListData));
    ListData* lData = dynamic_cast<ListData*>(data);
    if(pushDirection == LEFT_PUSH){
        lData->que_.push_front(value);
    }else{
        lData->que_.push_back(value);
    }
    dbmap_[key] = data;
    return lData->que_.size();
}

//加锁
std::string RedisServer::mapListPop(const std::string &key, int popDirection){
    std::lock_guard<std::mutex> lock(mutex_);
    DataPtr data = dbmap_[key];
    std::string ret;
    assert(data);
    assert(typeid(*data) == typeid(ListData));
    ListData* lData = dynamic_cast<ListData*>(data);
    if(popDirection == LEFT_POP){
        ret = lData->que_.front();
        lData->que_.pop_front();
    }else{
        ret = lData->que_.back();
        lData->que_.pop_back();
    }
    return ret;
}


//加锁
//使用这个函数一定要确保key有存在一个value,并且这个value是list
std::string RedisServer::mapListRange(const std::string &key, int left, int right){
    std::string ret;
    std::lock_guard<std::mutex> lock(mutex_);
    DataPtr data = dbmap_[key];
    assert(data != NULL);
    assert(typeid(*data) == typeid(ListData));
    ListData* ldata = dynamic_cast<ListData*> (data);
    std::deque<std::string>::iterator iter = ldata->que_.begin();
    if(iter == ldata->que_.end()){
        return RESULT_NULL;
    }
    int distanceTotal = distance(iter, ldata->que_.end());
    if(left < 0){
        left = distanceTotal + left;
    }

    if(right < 0){
        right = distanceTotal + right;
    }
    if(distanceTotal <= left){
        return RESULT_NULL;
    }
    advance(iter, left);

    if(distanceTotal <= right){
        right = distanceTotal - 1;
    }

    std::deque<std::string>::iterator iterRight = ldata->que_.begin();
    advance(iterRight, right + 1);
    //加锁的查找的操作应该不会遇到迭代器失效
    //所以这里放心定义迭代器
    while(iter != iterRight){
        ret = ret +"\"" + *iter + "\"\n";
        iter++;
    }
    return ret;
}

//加锁
//确保在调用这个函数之前,已经做了检查
int RedisServer::mapListRemove(const std::string& key,
                               int count, const std::string& value){
    int ret = count;
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = dbmap_.find(key);
    if(iter == dbmap_.end()){
        return -1;
    }
    DataPtr data = iter->second;
    assert(data != NULL);
    assert(typeid(*data) == typeid(ListData));
    ListData* lData = dynamic_cast<ListData*> (data);
    auto it = lData->que_.begin();
    while(count > 0 && it != lData->que_.end()){
        if(*it == value){
            it = lData->que_.erase(it);
            count--;
        }
         it++;
    }
    return ret;
}

//加锁
//确保调用这个函数之前,已经知道key确实有list的数据,否则会被assert炸
std::string RedisServer::mapListIndex(const std::string& key, int index){
    std::string ret;
    std::lock_guard<std::mutex> lock(mutex_);
    DataPtr data = dbmap_[key];
    assert(data);
    assert(typeid(*data) == typeid(ListData));
    ListData* lData = dynamic_cast<ListData*> (data);
    auto iter = lData->que_.begin();
    int distanceTotal = distance(iter, lData->que_.end());
    if(distanceTotal <= index){
        return RESULT_NULL;
    }
    advance(iter, index);
    return "\"" + *iter + "\"\n";
}

//加锁
//确保之前检查过存在list
int RedisServer::mapListInsert(const std::string& key,
                               const std::string& targetValue, const std::string& insertValue, int pos){
    std::lock_guard<std::mutex> lock(mutex_);
    DataPtr data = dbmap_[key];
    assert(data);
    assert(typeid(*data) == typeid(ListData));
    ListData* lData = dynamic_cast<ListData*>(data);
    std::deque<std::string>::iterator iter = lData->que_.begin();
    std::find(iter, lData->que_.end(), targetValue);
    if(iter == lData->que_.end()){
        return -1;
    }

    if(pos == LEFT_PUSH){
        lData->que_.insert(iter, insertValue);
    }else{
        lData->que_.insert(++iter,insertValue);
    }
    return lData->que_.size();
}

std::string RedisServer::mapGetType(const std::string &key)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = dbmap_.find(key);
    if(iter == dbmap_.end()){
        return "NULL";
    }
    DataPtr data = iter->second;
    if(typeid(*data) == typeid(StringData)){
        StringData* sData = dynamic_cast<StringData*>(data);
        if(isDigit(sData->data_)){
            return "int";
        }
        return "string";
    }else if(typeid(*data) == typeid(ListData)){
        return "list";
    }else if(typeid(*data) == typeid(MapData)){
        return "hash";
    }else if(typeid(*data) == typeid(SetData)){
        return "set";
    }else{
        return "unknow";
    }

}

//加锁
//同上
int RedisServer::mapSetSet(const std::string& key, const std::string& value){
    std::lock_guard<std::mutex> lock(mutex_);
    DataPtr data = dbmap_[key];
    if(!data){
        data = new SetData();
        dbmap_[key] = data;
    }
    assert(typeid(*data) == typeid(SetData));
    SetData* sData = dynamic_cast<SetData*>(data);
    sData->uset_.insert(value);
    return 1;
}

bool RedisServer::mapSetRemove(const std::string &key, const std::string &value){
    std::lock_guard<std::mutex> lock(mutex_);
    DataPtr data = dbmap_[key];
    assert(data);
    assert(typeid(*data) == typeid(SetData));
    SetData* sData = dynamic_cast<SetData*>(data);
    auto iter = sData->uset_.find(value);
    if(iter == sData->uset_.end()){
        return false;
    }
    sData->uset_.erase(iter);
    return true;

}

bool RedisServer::mapSetIsMember(const std::string &key, const std::string &value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    DataPtr data = dbmap_[key];
    assert(data);
    assert(typeid(*data) == typeid(SetData));
    SetData* sData = dynamic_cast<SetData*>(data);
    auto iter = sData->uset_.find(value);
    if(iter == sData->uset_.end()){
        return false;
    }
    return true;
}

std::string RedisServer::mapSetAllMembers(const std::string &key){
    std::string ret;
    std::lock_guard<std::mutex> lock(mutex_);
    DataPtr data = dbmap_[key];
    assert(data);
    assert(typeid(*data) == typeid(SetData));
    SetData* sData = dynamic_cast<SetData*>(data);
    if(sData->uset_.size() == 0){
        return RESULT_NULL;
    }
    for(auto iter =  sData->uset_.begin();
        iter != sData->uset_.end();iter++){
        ret += "\"" + *iter + "\"\n";
    }
    return ret;
}


std::string RedisServer::mapSetOp(const std::string &key1, const std::string &key2, int op) {
    std::string ret;
    std::vector<std::string> vec;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        DataPtr data1 = dbmap_[key1];
        DataPtr data2 = dbmap_[key2];
        assert(data1 && data2);
        assert(typeid(*data1) == typeid(SetData));
        assert(typeid(*data2) == typeid(SetData));
        SetData* sData1 = dynamic_cast<SetData*>(data1);
        SetData* sData2 = dynamic_cast<SetData*>(data2);
        auto iterInsert = std::inserter(vec, vec.begin());

        if(op == OP_UNION){
            std::set_union(sData1->uset_.begin(), sData1->uset_.end(),
                           sData2->uset_.begin(), sData2->uset_.end(), iterInsert);
        }else if( op == OP_INTER){
            std::set_intersection(sData1->uset_.begin(), sData1->uset_.end(),
                                  sData2->uset_.begin(), sData2->uset_.end(), iterInsert);
        }else if(op == OP_DIFF){
            std::set_difference(sData1->uset_.begin(), sData1->uset_.end(),
                                sData2->uset_.begin(), sData2->uset_.end(), iterInsert);
        }else{

        }
    }

    if(vec.size() == 0){
        return RESULT_NULL;
    }
    for(auto iter = vec.begin();
        iter != vec.end();iter++){
        ret += "\"" + *iter + "\"\n";
    }
    return ret;
}

}
