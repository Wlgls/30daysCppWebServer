#include "Acceptor.h"
#include "Channel.h"
#include "EventLoop.h"

#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <assert.h>
#include <cstring>
#include <fcntl.h>
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

void Acceptor::Create(){
    assert(listenfd_ == -1);
    listenfd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if(listenfd_ == -1){
        std::cout << "Failed to create socket" << std::endl;
    }
}

void Acceptor::Bind(const char *ip, const int port){
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    if(::bind(listenfd_, (struct sockaddr *)&addr, sizeof(addr))==-1){
        std::cout << "Failed to Bind : "  << ip << ":" << port << std::endl;
    }
}

void Acceptor::Listen(){
    assert(listenfd_ != -1);
    if(::listen(listenfd_, SOMAXCONN) == -1){
        std::cout << "Failed to Listen" << std::endl;
    }
}

void Acceptor::AcceptConnection(){
    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    assert(listenfd_ != -1);

    int clnt_fd = ::accept4(listenfd_, (struct sockaddr *)&client, &client_addrlength, SOCK_NONBLOCK | SOCK_CLOEXEC);
    
    if (clnt_fd == -1){
        std::cout << "Failed to Accept" << std::endl;
    }
    if(new_connection_callback_){
        new_connection_callback_(clnt_fd);
    }
}

void Acceptor::set_newconnection_callback(std::function<void(int)> const &callback){
    new_connection_callback_ = std::move(callback);
}
