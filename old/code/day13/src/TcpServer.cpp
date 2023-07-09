#include "TcpServer.h"
#include <unistd.h>
#include <functional>
#include "Acceptor.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "ThreadPool.h"
#include "util.h"
#include<iostream>



TcpServer::TcpServer(EventLoop *loop): main_reactor_(loop), acceptor_(nullptr), thread_pool_(nullptr){
    ErrorIf(main_reactor_ == nullptr, "main reactor can't be nullptr");
    acceptor_ = new Acceptor(main_reactor_);

    std::function<void(Socket *)> cb = std::bind(&TcpServer::NewConnection, this, std::placeholders::_1);
    acceptor_->SetNewConnectionCallback(cb);

    int size = static_cast<int>(std::thread::hardware_concurrency());
    thread_pool_ = new ThreadPool(size);
    for (int i = 0; i < size; ++i) {
        sub_reactors_.push_back(new EventLoop());
    }
    
    for (int i = 0; i < size; ++i){
        std::function<void()> sub_loop = std::bind(&EventLoop::Loop, sub_reactors_[i]);
        thread_pool_->Add(std::move(sub_loop));
    }
}

TcpServer::~TcpServer(){
    for(auto *each : sub_reactors_){
        delete each;
    }
    delete acceptor_;
    delete thread_pool_;
}

void TcpServer::NewConnection(Socket *sock){
    ErrorIf(sock->GetFd() == -1, "New Connection Err");
    uint64_t random = sock->GetFd() % sub_reactors_.size();
    TcpConnection *conn = new TcpConnection(sub_reactors_[random], sock);

    std::function<void(Socket *)> cb = std::bind(&TcpServer::DeleteConnection, this, std::placeholders::_1);

    conn->SetDeleteConnectionCallback(cb);
    conn->SetOnMessageCallback(on_message_callback_);
    connections_[sock->GetFd()] = conn;
    if(new_connect_callback_){
        new_connect_callback_(conn);
    }
}

void TcpServer::DeleteConnection(Socket *sock) {
    int sockfd = sock->GetFd();
    //if (sockfd!=-1){
    auto it = connections_.find(sockfd);
    if (it != connections_.end()) {
        TcpConnection *conn = connections_[sockfd];
        connections_.erase(sockfd);
        //delete conn; // 当不是正常关闭时，会引起sermentation fault
        conn = nullptr;
    }
    //}
    
}


void TcpServer::OnConnect(std::function<void(TcpConnection *)> fn) { on_connect_callback_ = std::move(fn); }

void TcpServer::OnMessage(std::function<void(TcpConnection *)> fn) { on_message_callback_ = std::move(fn); }

void TcpServer::NewConnect(std::function<void(TcpConnection *)> fn) { new_connect_callback_ = std::move(fn); }