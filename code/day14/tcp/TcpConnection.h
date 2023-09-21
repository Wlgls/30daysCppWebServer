#pragma once
#include "common.h"

#include <functional>
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

    
    TcpConnection(EventLoop *loop, int connfd, int connid);
    ~TcpConnection();

     // 关闭时的回调函数
    void set_close_callback(std::function<void(int)> const &fn);   
    // 接受到信息的回调函数                                  
    void set_message_callback(std::function<void(TcpConnection *)> const &fn); 


    // 设定send buf
    void set_send_buf(const char *str); 
    Buffer *read_buf();
    Buffer *send_buf();

    void Read(); // 读操作
    void Write(); // 写操作
    void Send(const std::string &msg); // 输出信息
    void Send(const char *msg, int len); // 输出信息
    void Send(const char *msg);


    void HandleMessage(); // 当接收到信息时，进行回调

    // 当TcpConnection发起关闭请求时，进行回调，释放相应的socket.
    void HandleClose(); 


    ConnectionState state() const;
    EventLoop *loop() const;
    int fd() const;
    int id() const;

private:
    // 该连接绑定的Socket
    int connfd_;
    int connid_;
    // 连接状态
    ConnectionState state_;

    EventLoop *loop_;

    std::unique_ptr<Channel> channel_;
    std::unique_ptr<Buffer> read_buf_;
    std::unique_ptr<Buffer> send_buf_;

    std::function<void(int)> on_close_;
    std::function<void(TcpConnection *)> on_message_;

    void ReadNonBlocking();
    void WriteNonBlocking();
};