#include "Acceptor.h"

#include<fcntl.h>
#include <utility>
#include "Channel.h"
#include "Socket.h"
#include "common.h"
#include <assert.h>


Acceptor::Acceptor(EventLoop *loop){
    socket_ = std::make_unique<Socket>();
    
    assert(socket_->Create() == RC_SUCCESS);
    assert(socket_->Bind("127.0.0.1", 1234) == RC_SUCCESS);
    assert(socket_->Listen() == RC_SUCCESS);

    channel_ = std::make_unique<Channel>(socket_->fd(), loop);
    std::function<void()> cb = std::bind(&Acceptor::AcceptConnection, this);

    channel_->set_read_callback(cb);
    channel_->EnableRead();
}

Acceptor::Acceptor(EventLoop *loop, const char *ip, const int port){
    socket_ = std::make_unique<Socket>();
    
    assert(socket_->Create() == RC_SUCCESS);
    assert(socket_->Bind(ip, port) == RC_SUCCESS);
    assert(socket_->Listen() == RC_SUCCESS);

    channel_ = std::make_unique<Channel>(socket_->fd(), loop);
    std::function<void()> cb = std::bind(&Acceptor::AcceptConnection, this);

    channel_->set_read_callback(cb);
    channel_->EnableRead();
}



Acceptor::~Acceptor(){};

RC Acceptor::AcceptConnection() const{
    int clnt_fd = -1;
    if (socket_->Accept(clnt_fd) != RC_SUCCESS){
        perror("Acceptor accept new connection failed ");
        return RC_ACCEPTOR_ERROR;
    }
    fcntl(clnt_fd, F_SETFL, fcntl(clnt_fd, F_GETFL) | O_NONBLOCK); // 新接受到的连接设置为非阻塞式
    if(new_connection_callback_){
        new_connection_callback_(clnt_fd);
    }
    return RC_SUCCESS;
}


void Acceptor::set_newconnection_callback(std::function<void(int)> const &callback){
    new_connection_callback_ = std::move(callback);
}