#include "Acceptor.h"

#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <assert.h>
#include <cstring>
#include <fcntl.h>
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

void Acceptor::Create(){
    assert(listenfd_ == -1);
    listenfd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if(listenfd_ == -1){
        std::cout << "Failed to create socket";
    }
}

void Acceptor::Bind(const char *ip, const int port) const{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    if(::bind(listenfd_, (struct sockaddr *)&addr, sizeof(addr))==-1){
        std::cout << "Failed to Bind";
    }
}

void Acceptor::Listen() const{
    assert(listenfd_ != -1);
    if(::listen(listenfd_, SOMAXCONN) == -1){
        std::cout << "Failed to Listen";
    }
}

void Acceptor::AcceptConnection(int &clnt_fd) const{
    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    assert(listenfd_ != -1);
    clnt_fd = ::accept4(listenfd_, (struct sockaddr *)&client, &client_addrlength, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (clnt_fd == -1){
        std::cout << "Failed to Accept";
    }
    
    struct sockaddr_in peeraddr;
    socklen_t peer_addrlength = sizeof(peeraddr);
    getpeername(clnt_fd, (struct sockaddr *)&peeraddr, &peer_addrlength);

    std::cout << "TcpServer::OnNewConnection : new connection "
             << "[fd#" << clnt_fd << "]"
             << " from " << inet_ntoa(peeraddr.sin_addr) << ":" << ntohs(peeraddr.sin_port);

    if(new_connection_callback_){
        new_connection_callback_(clnt_fd);
    }
}

void Acceptor::set_newconnection_callback(std::function<void(int)> const &callback){
    new_connection_callback_ = std::move(callback);
}