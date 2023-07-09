#include "TcpServer.h"
#include "Socket.h"
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
    std::function<void(int)> cb = std::bind(&TcpServer::OnNewConnection, this, std::placeholders::_1);
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
    std::function<void(int)> cb = std::bind(&TcpServer::OnNewConnection, this, std::placeholders::_1);
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

RC TcpServer::OnNewConnection(int fd){
    assert(fd != -1);
    uint64_t random = fd % sub_reactors_.size();

    std::shared_ptr<TcpConnection> conn = std::make_shared<TcpConnection>(sub_reactors_[random].get(), fd);
    std::function<void(int)> cb = std::bind(&TcpServer::OnClose, this, std::placeholders::_1);
    conn->set_connection_callback(on_connect_);
    conn->ConnectionEstablished();
    conn->set_close_callback(cb);
    conn->set_message_callback(on_message_);
    connectionsMap_[fd] = std::move(conn);
    return RC_SUCCESS;
}

RC TcpServer::OnClose(int fd){
    printf("Remove connection fd:%d\n\n", fd);
    auto it = connectionsMap_.find(fd);
    assert(it != connectionsMap_.end());
    connectionsMap_.erase(fd);
    return RC_SUCCESS;
}

void TcpServer::set_connection_callback(std::function<void(const std::shared_ptr<TcpConnection> &)> const &fn) { on_connect_ = std::move(fn); };
void TcpServer::set_message_callback(std::function<void(const std::shared_ptr<TcpConnection> &)> const &fn) { on_message_ = std::move(fn); };
