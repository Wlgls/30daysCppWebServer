#include "TcpServer.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "ThreadPool.h"
#include "common.h"
#include <unistd.h>
#include <iostream>
// #include <memory>
#include <assert.h>


TcpServer::TcpServer(const char * ip, const int port): next_conn_id_(1){
    main_reactor_ = std::make_unique<EventLoop>();
    acceptor_ = std::make_unique<Acceptor>(main_reactor_.get(), ip, port);
    std::function<void(int)> cb = std::bind(&TcpServer::HandleNewConnection, this, std::placeholders::_1);
    acceptor_->set_newconnection_callback(cb);

    unsigned int size = std::thread::hardware_concurrency();
    thread_pool_ = std::make_unique<ThreadPool>(size);

    for (size_t i = 0; i < size; ++i){
        std::unique_ptr<EventLoop> sub_reactor = std::make_unique<EventLoop>();
        sub_reactors_.push_back(std::move(sub_reactor));
    }
}

TcpServer::~TcpServer(){

};

void TcpServer::Start(){
    for (size_t i = 0; i < sub_reactors_.size(); ++i){
        std::function<void()> sub_loop = std::bind(&EventLoop::Loop, sub_reactors_[i].get());
        thread_pool_->Add(std::move(sub_loop));
    }
    main_reactor_->Loop();
}

void TcpServer::HandleNewConnection(int fd){
    
    assert(fd != -1);
    std::cout << "New connection fd: " << fd << std::endl;
    uint64_t random = fd % sub_reactors_.size();

    TcpConnection *conn = new TcpConnection(sub_reactors_[random].get(), fd, next_conn_id_);
    std::function<void(int)> cb = std::bind(&TcpServer::HandleClose, this, std::placeholders::_1);

    conn->set_close_callback(cb);
    conn->set_message_callback(on_message_);

    connectionsMap_[fd] = conn;
    // 分配id
    ++next_conn_id_;
    if(next_conn_id_ == 1000){
        next_conn_id_ = 1;
    }
}

void TcpServer::HandleClose(int fd){
    auto it =  connectionsMap_.find(fd);
    assert(it != connectionsMap_.end());
    TcpConnection * conn = connectionsMap_[fd];
    connectionsMap_.erase(fd);
    // delete conn;
    // 没有析构，所以在这里进行了 close以先关闭连接
    ::close(fd);
    conn = nullptr;
}

void TcpServer::set_connection_callback(std::function<void(TcpConnection *)> const &fn) { on_connect_ = std::move(fn); };
void TcpServer::set_message_callback(std::function<void(TcpConnection *)> const &fn) { on_message_ = std::move(fn); };
