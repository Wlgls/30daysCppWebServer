#include "Acceptor.h"

#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <assert.h>
#include <cstring>
#include<fcntl.h>
#include "Channel.h"
#include "common.h"
#include "EventLoop.h"
#include <assert.h>
#include <iostream>

Acceptor::Acceptor(EventLoop *loop, const char * ip, const int port) :loop_(loop), listenfd_(-1){
    Create();
    Bind(ip, port);
    Listen();
    channel_ = std::make_unique<Channel>(listenfd_, loop);
    std::function<void()> cb = std::bind(&Acceptor::AcceptConnection, this);
    channel_->set_read_callback(cb);
    channel_->EnableRead();
}


Acceptor::~Acceptor(){
    loop_->DeleteChannel(channel_.get());
    ::close(listenfd_);
};

RC Acceptor::Create(){
    assert(listenfd_ == -1);
    listenfd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if(listenfd_ == -1){
        perror("Failed to create socket");
        return RC_SOCKET_ERROR;
    }
    return RC_SUCCESS;
}

RC Acceptor::Bind(const char *ip, const int port) const{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    if(::bind(listenfd_, (struct sockaddr *)&addr, sizeof(addr))==-1){
        perror("faild to bind socket");
        return RC_SOCKET_ERROR;
    }
    return RC_SUCCESS;
}

RC Acceptor::Listen() const{
    assert(listenfd_ != -1);
    if(::listen(listenfd_, SOMAXCONN) == -1){
        perror("Failed to listen socket");
        return RC_SOCKET_ERROR;
    }
    return RC_SUCCESS;
}

RC Acceptor::Accept(int &clnt_fd) const{
    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    assert(listenfd_ != -1);
    clnt_fd = ::accept4(listenfd_, (struct sockaddr *)&client, &client_addrlength, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (clnt_fd == -1)
    {
        perror("Failed to accept socket");
        return RC_SOCKET_ERROR;
    }
    return RC_SUCCESS;
}



RC Acceptor::AcceptConnection() const{

    int clnt_fd = -1;
    if (Accept(clnt_fd) != RC_SUCCESS){
        perror("Acceptor accept new connection failed ");
        return RC_ACCEPTOR_ERROR;
    }
    //fcntl(clnt_fd, F_SETFL, fcntl(clnt_fd, F_GETFL) | O_NONBLOCK); // 新接受到的连接设置为非阻塞式
    
    struct sockaddr_in peeraddr;
    socklen_t peer_addrlength = sizeof(peeraddr);
    getpeername(clnt_fd, (struct sockaddr *)&peeraddr, &peer_addrlength);

    std::cout << "TcpServer::OnNewConnection :"
              << "from: " << inet_ntoa(peeraddr.sin_addr) << ":" << ntohs(peeraddr.sin_port) << " " << "fd: " << clnt_fd << std::endl;

    if(new_connection_callback_){
        new_connection_callback_(clnt_fd);
    }
    return RC_SUCCESS;
}


void Acceptor::set_newconnection_callback(std::function<void(int)> const &callback){
    new_connection_callback_ = std::move(callback);
}