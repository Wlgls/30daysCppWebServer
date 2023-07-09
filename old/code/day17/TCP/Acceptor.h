/*
用于接收一个新建连接

在这个模块中，主要是建立一个server socket，并监听他，当有新的连接建立时，将根据传入的回调函数对客户端sock进行操作。
而本身回调函数本身放在了server中。


这个代码似乎是不完善的，析构似乎并不太准确，当acceptor被析构时，没有做任何操作，正常情况下，应该将构造的channel从eoller中取消，并关闭当前socket。

*/


#pragma once


#include "common.h"
#include <memory>
#include <functional>

class Socket;
class Channel;

class Acceptor
{
public:
    DISALLOW_COPY_AND_MOVE(Acceptor);
    Acceptor(EventLoop *loop);
    Acceptor(EventLoop *loop, const char *ip, const int port);
    ~Acceptor();

    RC AcceptConnection() const;
    void set_newconnection_callback(std::function<void(int)> const &callback);

private:
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    std::function<void(int)> new_connection_callback_;
};