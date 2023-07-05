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
class TcpConnection
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

    void set_connect_callback(std::function<void(TcpConnection *)> const &fn);
    void set_close_callback(std::function<void(int)> const &fn);               // 关闭时的回调函数
    void set_message_callback(std::function<void(TcpConnection *)> const &fn); // 接受到信息的回调函数

    void set_send_buf(const char *str); // 设定send buf
    Buffer *read_buf();
    Buffer *send_buf();

    void Read(); // 读操作
    void Write(); // 写操作
    void Send(std::string &msg); // 输出信息
    void Send(const char *msg, int len); // 输出信息
    void Send(const char *msg);

    Socket *socket() const;

    void OnMessage(); // 当接收到信息时，进行回调
    void OnClose(); // 关闭时，进行回调

    ConnectionState state() const;

private:
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;

    ConnectionState state_;

    std::unique_ptr<Buffer> read_buf_;
    std::unique_ptr<Buffer> send_buf_;

    std::function<void(int)> on_close_;
    std::function<void(TcpConnection *)> on_connect_;
    std::function<void(TcpConnection *)> on_message_;

    void ReadNonBlocking();
    void WriteNonBlocking();
};