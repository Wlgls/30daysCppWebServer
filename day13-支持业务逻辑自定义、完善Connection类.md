# day13-支持业务自定义、完善Connection类

* 由于最开始是照着源仓库敲的，部分函数名发生了改变，因此注意其他类的部分信息改变

当前的echo server是定义在`Connection`类中的。当我们需要不同的业务逻辑时，我们需要直接改动`Connection`类中的代码，这不太合理。我们期望将网络库与业务逻辑进行切分，由用户自行定义业务逻辑。因此今天主要完善相应的Connection类。

首先为了定义业务逻辑，一个简单的`echo server`被定义为:

```c++
class EchoServer{
    public:
        EchoServer(EventLoop *loop);
        ~EchoServer();

        void start();
        void onConnection(TcpConnection *conn);
        void onMessage(TcpConnection *conn);

    private:
        EventLoop *loop_;
        TcpServer server_;
};
```

在`echo server`中，我们需要定义建立连接`onConnection`和接受数据`onMessage`时需要做什么，并将设置为`tcp server`的回调函数， 回调函数参数是一个`TcpConnection`的指针，代表着服务器到客户端的连接，在客户端就可以写业务逻辑。最终这个函数会被绑定在`TcpConnection`类中的`on_connect_callback_`,也就是`Channel`类处理的事件(这个版本只考虑了可读事件)。

这样我们的`TcpConnection`类就可以专注于从socket中读写数据，抛弃具体的业务逻辑。具体的，`TcpConnection`类提供了两个函数，分别是`Write()`和`Read()`。`Write()`函数表示将`write_buf_`中的内容发送到该`TcpConnection`的`sockfd`中，`Read()`则是将TCP缓冲区中数据读到读缓冲区中。

```c++
class TcpConnection{
    public:
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
}
```

为了方便对`TcpConnection`的操作，当`TcpServer`接受到一个新连接时，设定`TcpConnection`的相应函数

```c++
void TcpServer::NewConnection(Socket *sock){
    ErrorIf(sock->GetFd() == -1, "New Connection Err");
    uint64_t random = sock->GetFd() % sub_reactors_.size();
    TcpConnection *conn = new TcpConnection(sub_reactors_[random], sock);

    std::function<void(Socket *)> cb = std::bind(&TcpServer::DeleteConnection, this, std::placeholders::_1);

    conn->SetDeleteConnectionCallback(cb);
    conn->SetOnMessageCallback(on_message_callback_);
    connections_[sock->GetFd()] = conn;
    if(new_connect_callback_){
        new_connect_callback_(conn);
    }
}
```

