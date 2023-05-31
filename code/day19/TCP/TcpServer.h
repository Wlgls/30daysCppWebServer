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
    TcpServer(const char *ip, const int port);
    ~TcpServer();

    void Start();
    

    void set_connection_callback(std::function < void(const std::shared_ptr<TcpConnection> &)> const &fn);
    void set_message_callback(std::function < void(const std::shared_ptr<TcpConnection> &)> const &fn);

    RC OnClose(const std::shared_ptr<TcpConnection> &);
    RC OnNewConnection(int fd);

    private:
        int next_conn_id_;
        std::unique_ptr<EventLoop> main_reactor_;
        std::unique_ptr<Acceptor> acceptor_;
        // std::unordered_map<int, std::unique_ptr<TcpConnection>> connectionsMap_;
        std::map<int, std::shared_ptr<TcpConnection>> connectionsMap_;
        // std::unordered_map<int, TcpConnection *> connectionsMap_;
        std::vector<std::unique_ptr<EventLoop>> sub_reactors_;

        std::unique_ptr<ThreadPool> thread_pool_;

        std::function<void(const std::shared_ptr<TcpConnection> &)> on_connect_;
        std::function<void(const std::shared_ptr<TcpConnection> &)> on_message_;

};
