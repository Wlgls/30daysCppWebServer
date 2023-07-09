/*

对整个服务器的统辖管理。

采用Reactor模式。在这种模式中，main Reactor负责Acceptor建立连接，并将建立的连接分发到sub Reactor上。
*/

#pragma once

#include <functional>
#include<map>
#include<vector>

class EventLoop;
class Socket;
class Acceptor;
class TcpConnection;
class ThreadPool;
class TcpServer{
    private:
        EventLoop *main_reactor_;
        Acceptor *acceptor_;
        std::map<int, TcpConnection *> connections_;
        std::vector<EventLoop *> sub_reactors_;

        ThreadPool *thread_pool_;
        std::function<void(TcpConnection *)> on_connect_callback_;
        std::function<void(TcpConnection *)> on_message_callback_;
        std::function<void(TcpConnection *)> new_connect_callback_;

    public:
        explicit TcpServer(EventLoop *loop);
        ~TcpServer();

        void NewConnection(Socket *sock);
        void DeleteConnection(Socket *sock);

        void OnConnect(std::function<void(TcpConnection *)>  fn);
        void OnMessage(std::function<void(TcpConnection *)>  fn);
        void NewConnect(std::function<void(TcpConnection *)>  fn);
};