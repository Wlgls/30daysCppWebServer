/*
用于接收一个新建连接

在这个模块中，主要是建立一个server socket，并监听他，当有新的连接建立时，将根据传入的回调函数对客户端sock进行操作。
而本身回调函数本身放在了server中。
*/


#pragma once
#include<functional>

class EventLoop;
class Socket;
class InetAddress;
class Channel;
class Acceptor{
    private:
        EventLoop *loop_;
        Socket *sock_;
        Channel *acceptChannel_;
        std::function<void(Socket *)> new_connection_callback_;

    public:
        explicit Acceptor(EventLoop *_loop);
        ~Acceptor();

        void AcceptConnection();
        void SetNewConnectionCallback(std::function<void(Socket *)> const &callback);
};