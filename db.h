/*************************************************************************
  > File Name: db.h
  > Author: Sunfish
  > Mail: 704613309@qq.com
  > Created Time: 2016年07月27日 星期三 14时32分26秒
 ************************************************************************/
#ifndef __DB_DATA_H__
#define __DB_DATA_H__
#include <string>
#include <map>
#include <deque>
#include <set>
namespace libevent{
class Data{
public:
    virtual ~Data(){}
};

class StringData : public Data{
public:
    StringData():data_(){}
    virtual ~StringData(){}
    std::string data_;
};

class MapData : public Data{
public:
    MapData():map_(){}
    virtual ~MapData(){}
    std::map<std::string, std::string> map_;
};

class ListData :public Data{
public:
    ListData(): que_(){}
    virtual~ListData(){}
    std::deque<std::string> que_;
};

class SetData : public Data{
public:
    SetData():uset_(){}
    virtual ~SetData(){}
    std::set<std::string> uset_;
};
}


#endif
