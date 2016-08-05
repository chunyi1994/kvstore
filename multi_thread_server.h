
#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include <unistd.h>
#include <iostream>
#include <event.h>
#include <vector>
#include <event2/bufferevent.h>
#include <pthread.h>

namespace libevent{
    const short EVENT_CLOSE = 0;
    const short EVENT_NEW_CONNECTION = 1;

	const int THREADS_NUM = 4;
	const int BACKLOG = 10;
	class TcpServer;

	struct MultiThreadData{
		pthread_t tid_;
		int pipeFd_[2]; //fd[0]可以读,fd[1]可以写
		struct event_base* base_;
		TcpServer *server_;
	};
	
	class TcpServer{
		friend void cbAccept(int fd, short events, void*arg);
        friend void cbRead(bufferevent *bev, void* arg);
        friend void cbClose(bufferevent *bev,short events, void*arg);
        friend void cbThreadReadyForRead(int fd, short events, void* arg);
		public:
			TcpServer(size_t port);
			~TcpServer();

			void start();
			event_base* base() const;	
			void send(bufferevent *bev, const std::string &msg);
		
		protected:
            virtual void startCallback(){}
			//在派生类中覆盖一个自己的readCallback
			virtual void readCallback(bufferevent *bev, const std::string &msg);
            virtual void connectionCallback(bufferevent *bev,  short event){}

			size_t port_;

		private:
			int initTcpServer(int backlog);
			void handleRead(bufferevent *bev);
			void initThreads();
			std::vector<MultiThreadData> threadsData_;
			struct event_base* base_;
		

	};



}
#endif
