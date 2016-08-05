#include "multi_thread_server.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <utils.h>
namespace libevent{

void cbRead(bufferevent *bev, void* arg);
void cbClose(bufferevent* bev, short event, void *arg);
void cbThreadReadyForRead(int fd, short events, void* arg);
void cbAccept(int fd, short events, void*arg);
void* threadFunction(void *);

TcpServer::TcpServer(size_t port):
    port_(port),
    threadsData_(THREADS_NUM),
    base_(NULL)
{}

TcpServer::~TcpServer(){
    event_base_free(base_);
    for(size_t i = 0; i < threadsData_.size(); i++){
        event_base_free(threadsData_[i].base_);
    }
}

void TcpServer::initThreads(){
    for(int i = 0; i < THREADS_NUM; i++){
        threadsData_[i].base_ = NULL;
        threadsData_[i].server_= this;
        if(pipe(threadsData_[i].pipeFd_) < 0){
            std::cout<<"pipe err"<<std::endl;
        }
        pthread_create(&threadsData_[i].tid_, NULL, threadFunction,&threadsData_[i]);
    }
}

void TcpServer::start(){
    startCallback();
    evutil_socket_t listenfd = initTcpServer(BACKLOG);
    base_ = event_base_new();
    //添加监听客户端请求连接事件
    struct event* evAcceptor = event_new(base_, listenfd, EV_READ | EV_PERSIST, cbAccept, this);
    event_add(evAcceptor, NULL);
    initThreads();
    event_base_dispatch(base_);

}

event_base* TcpServer::base() const{
    return base_;
}

void TcpServer::handleRead(bufferevent *bev){
    char buf[1024];
    std::string msg;
    ssize_t n;
    while(1){
        n = bufferevent_read(bev, buf, sizeof(buf) - 1);
        if(n <= 0)break;
        buf[n] = '\0';
        std::string line(buf);
        msg.append(line);
    }
    readCallback(bev, msg);
}

void TcpServer::readCallback(bufferevent *buf, const std::string &msg){
}

void TcpServer::send(bufferevent *bev,const std::string &msg){
    bufferevent_write(bev, msg.c_str(), msg.length());
}

int TcpServer::initTcpServer(int backlog){
    evutil_socket_t listenfd;
    listenfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd == -1){
        log("Init TcpServer error.");
        return -1;
    }
    //允许多次绑定同一个地址。要用在socket和bind之间
    evutil_make_listen_socket_reuseable(listenfd);
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(port_);
    if(::bind(listenfd, (struct sockaddr*)&sin, sizeof(sin)) < 0){
        log("Bind error.");
        return -1;
    }
    if(::listen(listenfd, backlog) < 0){
         log("Listen error.");
        return -1;
    }
    //跨平台统一接口，将套接字设置为非阻塞状态
    evutil_make_socket_nonblocking(listenfd);
    return listenfd;
}

void cbThreadReadyForRead(int fd, short events, void* arg){
    char buf[11];
    ssize_t n = read(fd, buf, sizeof(buf));
    buf[n] = '\0';
    int sockfd = atoi(buf);
    MultiThreadData* data = (MultiThreadData *)arg;
    bufferevent *bev = bufferevent_socket_new(data->base_, sockfd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, cbRead, NULL, cbClose, data->server_);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);
    data->server_->connectionCallback(bev, EVENT_NEW_CONNECTION);
}

void* threadFunction(void *arg){
    MultiThreadData* data = (MultiThreadData*)arg;
    data->base_ = event_base_new();
    //注册一个event,这个event callback的功能是:
    //从管道中读取主线程发来的一个sockfd,
    //然后建立一个bufferevent来对这个sockfd进行消息传输
    struct event* evReadyForRead = event_new(data->base_, data->pipeFd_[0], EV_READ | EV_PERSIST, cbThreadReadyForRead, arg);
    event_add(evReadyForRead, NULL);
    event_base_dispatch(data->base_);
    return (void*)0;
}

void cbAccept(int fd, short events, void* arg){
    evutil_socket_t sockfd;
    socklen_t len;
    struct sockaddr_in client;
    sockfd = ::accept(fd, (struct sockaddr*)&client, &len);
    if(sockfd < 0){
         log("Accept error.");
         return;
    }
    evutil_make_socket_nonblocking(sockfd);
    //accpet了一个fd,
    //然后hash一下,通过管道分派给某一个线程
    //让分配好的线程来负责消息的接收和传递
    TcpServer *server = (TcpServer*) arg;
    int pos = sockfd % THREADS_NUM;
    std::stringstream ss;
    std::string fdstr;
    ss<<sockfd;
    ss>>fdstr;
    int pipeWriteFd = server->threadsData_[pos].pipeFd_[1];
    //fix me:不能确保100%发送成功
    int n = write(pipeWriteFd, fdstr.c_str(), fdstr.length());
    if(n < 0){
        std::cout<<"cbAccept :wirte err"<<std::endl;
    }
}

void cbRead(bufferevent *bev, void* arg){
    TcpServer* server = (TcpServer*)arg;
    server->handleRead(bev);
}

void cbClose(bufferevent* bev, short event, void *arg){
    if(event & BEV_EVENT_EOF){

    }
     TcpServer *server = (TcpServer*)arg;
     server->connectionCallback(bev, EVENT_CLOSE);
    //这将自动close套接字和free读写缓冲区
    bufferevent_free(bev);

}
}


