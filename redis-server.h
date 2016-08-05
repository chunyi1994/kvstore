/*************************************************************************
  > File Name: redis-server.h
  > Author: Sunfish
  > Mail: 704613309@qq.com 
  > Created Time: 2016年07月27日 星期三 09时35分02秒
 ************************************************************************/
#ifndef __REDIS_SERVER_H__
#define __REDIS_SERVER_H__

#include "db.h"
#include "multi_thread_server.h"
#include <iostream>
#include <map>
#include <typeinfo>
#include <mutex>
namespace libevent{
	typedef Data* DataPtr;
	typedef std::map<std::string, std::string> Map;	
	typedef std::map<std::string, DataPtr> DBMap;
	class RedisServer : public TcpServer{
		public:
			RedisServer(int port):TcpServer(port),dbmap_(){
			}

		protected:
			virtual void readCallback(bufferevent *, const std::string&);
            virtual void startCallback();
            virtual void connectionCallback(bufferevent *bev, short event);

		private:
			void processMsg(bufferevent *bev, std::vector<std::string> &vec);
			bool mapExists(const std::string &);
			Data* mapFind(const std::string &);
            bool mapFind(const std::string &, const std::type_info& info);
            bool mapRemove(const std::string&);
			//如果找到一个不是所期望类型的
			bool mapExceptTypeFind(const std::string&, const std::type_info& info);
			//string
            void mapSetString(const std::string &, const std::string &);
			bool mapFindString(const std::string &);
			std::string& mapGetString(const std::string &);
            std::string mapStringIncr(const std::string &key, const std::string& incr);
            //我也不知道怎么命名更好
            //我们的数据是一个map,并且是string, data键值对
            //如果data也是个map, 那就是mapGetMap
            //如果data是个list,那就是mapGetList
			//map
			Map& mapGetMap(const std::string & field);
			bool mapFindMap(const std::string& field);
			bool mapFindMapSecond(const std::string& field,const std::string& key);
			std::string& mapGetMapSecond(const std::string & value, const std::string & key);
            int mapSetMap(const std::string &field, const std::string& key, const std::string& value);
			bool mapMapRemove(const std::string& field, const std::string& key);
			std::string mapGetAllMapString(const std::string& field);
            std::string mapMapStringIncr(const std::string &field, const std::string& key, const std::string& incr);
            //set
			bool mapFindSet(const std::string& key);
            int mapSetSet(const std::string& key, const std::string& value);
			bool mapSetRemove(const std::string& key, const std::string& value);
            bool mapSetIsMember(const std::string& key, const std::string &value);
            std::string mapSetAllMembers(const std::string& key);
            std::string mapSetOp(const std::string& key1, const std::string& key2, int op);


			//list
            int mapListPush(const std::string &key, const std::string &value, int pushDirection);
            std::string mapListPop(const std::string &key,  int popDirection);
			std::string mapListRange(const std::string &key, int left, int right);
			bool mapFindList(const std::string& key);
			int mapListRemove(const std::string& key,int count, const std::string& value);
			std::string mapListIndex(const std::string& key, int index);
            int mapListInsert(const std::string& key,const std::string& target, const std::string& insert, int direction);
            std::string mapGetType(const std::string& key);
		
			//cmd
			void cmdExists(bufferevent* bev, const std::string& msg);
			void cmdGet(bufferevent* bev, const std::string& msg);
			void cmdSet(bufferevent* bev, const std::string& msg);
			void cmdDel(bufferevent* bev, const std::string& msg);
			void cmdIncrby(bufferevent* bev, const std::string& msg);
            void cmdType(bufferevent* bev, const std::string& msg);
			
			void cmdHset(bufferevent* bev, const std::string& msg);
			void cmdHget(bufferevent* bev, const std::string& msg);
			void cmdHexists(bufferevent* bev, const std::string& msg);
			void cmdHgetall(bufferevent* bev, const std::string& msg);
			void cmdHdel(bufferevent* bev, const std::string& msg);
			void cmdHincrby(bufferevent* bev, const std::string& msg);
			
			void cmdListPush(bufferevent* bev, const std::string& msg, int pushDirection);
			void cmdListPop(bufferevent* bev, const std::string& msg, int popDirection);
			void cmdLrange(bufferevent* bev, const std::string& msg);
			void cmdLrem(bufferevent* bev, const std::string& msg);
			void cmdLindex(bufferevent* bev, const std::string& msg);
			void cmdLinsert(bufferevent* bev, const std::string& msg);
			
			
			void cmdSrem(bufferevent* bev, const std::string& msg);
			void cmdSadd(bufferevent* bev, const std::string& msg);
            void cmdSismember(bufferevent* bev, const std::string& msg);
            void cmdSmembers(bufferevent* bev, const std::string& msg);
            void cmdSOp(bufferevent* bev, const std::string& msg, int op);

            mutable std::mutex mutex_;
			DBMap dbmap_;
	};


}

#endif
