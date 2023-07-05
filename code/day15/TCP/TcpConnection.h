/*
将tcp连接模块化。

在这个类中，主要是保存一个tcp连接。在这个类中，需要一个读缓冲区和一个写缓冲区。用于读取数据和发送数据。

此外，需要较多的回调函数，例如，需要对数据做什么操作，当建立新连接时需要做什么，怎么删除等操作。
*/


#pragma once
#include <functional>
#include "common.h"
#include <memory>
#include <string>
class Buffer;
class TcpConnection : public  std::enable_shared_from_this<TcpConnection>
{
public:
    enum ConnectionState
    {
        Invalid = 1,
        Connected,
        Disconected
    };
    DISALLOW_COPY_AND_MOVE(TcpConnection);
    TcpConnection(EventLoop *loop, int connfd);
    ~TcpConnection();

    void ConnectionEstablished();
    void ConnectionDestructor();
    void set_connection_callback(std::function<void(const std::shared_ptr<TcpConnection> &)> const &fn);
    void set_close_callback(std::function<void(const std::shared_ptr<TcpConnection> &)> const &fn);                                      // 关闭时的回调函数
    void set_message_callback(std::function<void(const std::shared_ptr<TcpConnection> &)> const &fn); // 接受到信息的回调函数                                   // 关闭时的回调函数

    void set_send_buf(const char *str); // 设定send buf
    Buffer *read_buf();
    Buffer *send_buf();

    RC Read(); // 读操作
    RC Write(); // 写操作
    RC Send(std::string &msg); // 输出信息
    RC Send(const char *msg, int len); // 输出信息
    RC Send(const char *msg);

    Socket *socket() const;

    void OnConnect(); 
    void OnMessage(); // 当接收到信息时，进行回调
    void OnClose(); // 关闭时，进行回调

    ConnectionState state() const;

    EventLoop *loop() const;    

private:

    EventLoop *loop_;

    std::unique_ptr<Socket> socket_;
    //Channel * channel_;
    std::unique_ptr<Channel> channel_;
    ConnectionState state_;

    std::unique_ptr<Buffer> read_buf_;
    std::unique_ptr<Buffer> send_buf_;

    std::function<void(const std::shared_ptr<TcpConnection> &)> on_close_;
    std::function<void(const std::shared_ptr<TcpConnection> &)> on_message_;
    std::function<void(const std::shared_ptr<TcpConnection> &)> on_connect_;

    RC ReadNonBlocking();
    RC WriteNonBlocking();
};