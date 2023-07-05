#include "TcpServer.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "ThreadPool.h"
#include "common.h"
// #include <memory>
#include <assert.h>

TcpServer::TcpServer(){
    main_reactor_ = std::make_unique<EventLoop>();
    acceptor_ = std::make_unique<Acceptor>(main_reactor_.get());
    std::function<void(int)> cb = std::bind(&TcpServer::HandleNewConnection, this, std::placeholders::_1);
    acceptor_->set_newconnection_callback(cb);

    unsigned int size = std::thread::hardware_concurrency();
    thread_pool_ = std::make_unique<ThreadPool>(size);

    for (size_t i = 0; i < size; ++i){
        std::unique_ptr<EventLoop> sub_reactor = std::make_unique<EventLoop>();
        sub_reactors_.push_back(std::move(sub_reactor));
    }
}

TcpServer::TcpServer(const char * ip, const int port){
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
    uint64_t random = fd % sub_reactors_.size();

    //TcpConnection *conn = new TcpConnection(sub_reactors_[random].get(), fd);
    std::unique_ptr<TcpConnection> conn = std::make_unique<TcpConnection>(sub_reactors_[random].get(), fd);
    std::function<void(int)> cb = std::bind(&TcpServer::HandleClose, this, std::placeholders::_1);
    conn->set_connection_callback(on_connect_);
    conn->ConnectionEstablished();
    conn->set_close_callback(cb);
    conn->set_message_callback(on_message_);
    //connectionsMap_[fd] = conn;
    connectionsMap_[fd] = std::move(conn);

    if(on_connect_){
        //on_connect_(connectionsMap_[fd]);
        on_connect_(connectionsMap_[fd].get());
    }
}

void TcpServer::HandleClose(int fd){
    auto it =  connectionsMap_.find(fd);
    assert(it != connectionsMap_.end());
    //TcpConnection * conn = connectionsMap_[fd];
    connectionsMap_.erase(fd);
    loop->QueueOneFunc(std::bind(&TcpConnection::ConnectionDestructor, conn));
    //conn = nullptr;
    // delete conn;
}

void TcpServer::set_connection_callback(std::function<void(TcpConnection *)> const &fn) { on_connect_ = std::move(fn); };
void TcpServer::set_message_callback(std::function<void(TcpConnection *)> const &fn) { on_message_ = std::move(fn); };
