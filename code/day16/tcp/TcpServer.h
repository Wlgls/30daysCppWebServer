#pragma once
#include "common.h"
#include <functional>
#include <map>
#include <vector>
#include <memory>
class EventLoop;
class TcpConnection;
class Acceptor;
class ThreadPool;
class InetAddress;
class TcpServer
{
    public:
    DISALLOW_COPY_AND_MOVE(TcpServer);
    TcpServer(EventLoop *loop, const char *ip, const int port);
    ~TcpServer();

    void Start();

    void set_connection_callback(std::function < void(const std::shared_ptr<TcpConnection> &)> const &fn);
    void set_message_callback(std::function < void(const std::shared_ptr<TcpConnection> &)> const &fn);

    inline void HandleClose(const std::shared_ptr<TcpConnection> &);
    // 进行一层额外的封装，以保证erase操作是由`main_reactor_`来操作的。
    inline void HandleCloseInLoop(const std::shared_ptr<TcpConnection> &);

    // 接收到消息做的操作。
    inline void HandleNewConnection(int fd);

    private:
        EventLoop *main_reactor_;
        int next_conn_id_;
    
        std::unique_ptr<Acceptor> acceptor_;
        std::vector<std::unique_ptr<EventLoop>> sub_reactors_;

        std::map<int, std::shared_ptr<TcpConnection>> connectionsMap_;
        std::unique_ptr<ThreadPool> thread_pool_;
        std::function<void(const std::shared_ptr<TcpConnection> &)> on_connect_;
        std::function<void(const std::shared_ptr<TcpConnection> &)> on_message_;

};
