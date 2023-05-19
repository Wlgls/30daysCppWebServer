/*
将tcp连接模块化。

在这个类中，主要是保存一个tcp连接。在这个类中，需要一个读缓冲区和一个写缓冲区。用于读取数据和发送数据。

此外，需要较多的回调函数，例如，需要对数据做什么操作，当建立新连接时需要做什么，怎么删除等操作。
*/

#pragma once
#include <functional>
#include <string>

class EventLoop;
class Socket;
class Channel;
class Buffer;
class TcpConnection{
    public:
        enum State
        {
            Invalid = 1,
            Connecting,
            Connected,
            Closed,
            Faild,
        };
        TcpConnection(EventLoop *loop, Socket *sock);
        ~TcpConnection();

        void Read();
        void Write();
        void Send(std::string msg);

        void SetDeleteConnectionCallback(std::function<void(Socket *)> const &callback);
        void SetOnConnectCallback(std::function<void(TcpConnection *)> const &callback);
        void SetOnMessageCallback(std::function<void(TcpConnection *)> const &callback);

        void Business();

        State GetState(); // 获取状态
        void Close();

        void SetSendBuffer(const char *str); // 将数据加入到发送缓冲区中
        Buffer *GetReadBuffer();
        const char *ReadBuffer();

        Buffer *GetSendBuffer();
        const char *SendBuffer();
        void GetlineSendBuffer();
        Socket *GetSocket();


    private:
        EventLoop *loop_;
        Socket *sock_;
        Channel *channel_{nullptr};
        State state_{State::Invalid};
        Buffer *read_buffer_{nullptr};
        Buffer *send_buffer_{nullptr};

        std::function<void(Socket *)> delete_connection_callback_;
        std::function<void(TcpConnection *)> on_connect_callback_;
        std::function<void(TcpConnection *)> on_message_callback_;

        void ReadNonBlocking();
        void WriteNonBlocking();
};