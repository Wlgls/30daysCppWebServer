# day13-重构核心库、使用智能指针

* 在断开连接时，会发生内存泄漏。

之前的操作已经对完成了一个分离业务逻辑的简单网络库，但是整个项目越来越复杂，模块越来越多。为了弥补之前的设计、细节缺陷。应该对程序进行重构。

本次重构主要包含以下几个方面：

* 使用更加容易理解的函数名。减少无用的函数，使代码看起来更简洁，可读。
* 进行内存管理。在之前的操作所有的内存都是由裸指针进行处理的，在类的构造阶段分配内存，析构释放内存，为了更加方便的管理内存，对于类自己拥有的资源使用了智能指针`std::unique_ptr<>`进行管理，对于不属于自己的资源，但是借用的资源，使用裸指针进行处理。
* 避免资源的复制操作 ，尽量使用移动语义进行所有权转移，以提升程序性能。

为了方便后续的工作，对所有的模块进行了梳理和总结，以便于下一步工作。只列出了每个类的工作，对于详细的代码没有列出。

`Socket`主要是对套接字socket的操作的封装。对于服务器而言，我们需要首先创建一个socket，并将其绑在在一个ip地址和端口上。之后监听这个套接字。并在之后接受客户端并连接。

```c++
#pragma once
#include <string>
#include <cstring>
#include "common.h"

class Socket{
    public:
        DISALLOW_COPY_AND_MOVE(Socket);

        Socket();
        ~Socket();

        void SetFd(int fd);
        int fd() const;

        std::string get_addr() const;

        RC Create(); // 创建socket
        RC Bind(const char *ip, uint16_t port) const; // 绑定ip地址
        RC Listen() const;             // 监听
        RC Accept(int &clnt_fd) const; // 接受客户端

        RC Connect(const char *ip, uint16_t port) const; // 连接
        RC SetNonBlocking() const; // 设置非阻塞

        bool IsNonBlocking() const; // 判断是否是阻塞

    private:
        int fd_;
};
```

`Channel`类是网络库的核心组建之一，他对`socket`进行了更深度的封装，保存了我们需要对`socket`监听的事件，当前`socket`已经准备好的事件并进行处理。此外，为了更新和获取在`epoller`中的状态，需要使用`EventLoop`进行管理，由于只是使用`EventLoop`，因此采用裸指针进行内存管理。

```c++
#pragma once

#include <functional>
#include "common.h"

class EventLoop;
class Channel {
    public:
        DISALLOW_COPY_AND_MOVE(Channel);
        Channel(int fd, EventLoop * loop);
        
        ~Channel();

        void HandleEvent() const; // 处理事件
        void EnableRead(); // 允许读
        void EnableWrite(); // 允许写
        void EnableET(); // 以ET形式触发


        int fd() const;  // 获取fd
        short listen_events() const; // 监听的事件
        short ready_events() const; // 准备好的事件

        bool IsInEpoll() const; // 判断当前channel是否在poller中
        void SetInEpoll(bool in = true); // 设置当前状态为poller中
        

        void SetReadyEvents(short ev);
        void set_read_callback(std::function<void()> const &callback);// 设置回调函数
        void set_write_callback(std::function<void()> const &callback);

        static const short READ_EVENT;    // 事件定义
        static const short WRITE_EVENT;  
        static const short ET;           
    private:
        int fd_;
        EventLoop *loop_;
        
        short listen_events_;
        short ready_events_;
        bool in_epoll_{false};
        std::function<void()> read_callback_;
        std::function<void()> write_callback_;
};
```

针对`Epoller`类，主要是进行IO多路复用，保证高并发。在`Epoller`类主要是对`epoll`中`channel`的监听与处理。

```C++
class Epoller
{
public:
    DISALLOW_COPY_AND_MOVE(Epoller);

    Epoller();
    ~Epoller();

    RC UpdateChannel(Channel *ch) const;
    RC DeleteChannel(Channel *ch) const;

    std::vector<Channel *> Poll(long timeout = -1) const;

    private:
        int fd_;
        struct epoll_event *events_;
};
```

之后，对`Eentloop`进行了定义，该类是对事件的轮询和处理。由于每一个`EventLoop`都有一个独属于自己的`Epoller`，因此采用了智能指针进行了管理

```c++
class EventLoop
{
public:
    DISALLOW_COPY_AND_MOVE(EventLoop);
    EventLoop();
    ~EventLoop();

    void Loop() const;
    void UpdateChannel(Channel *ch) const;
    void DeleteChannel(Channel *ch) const;

private:
    std::unique_ptr<Epoller> poller_;
};
```

之后创建了`Acceptor`作为一个接收器，用于服务器接收连接，并在接受连接之后进行相应的处理。这个类需要独属于自己的`Socket`和`Channel`，因此采用了智能指针管理。

```c++
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
```

对于每个TCP连接，都可以使用一个类进行管理，在这个类中，将注意力转移到对客户端`socket`的读写上，除此之外，他还需要绑定几个回调函数，例如当接收到信息时，或者需要关闭时进行的操作。

```c++
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

    void set_close_callback(std::function<void(int)> const &fn); // 关闭时的回调函数
    void set_message_callback(std::function<void(TcpConnection *)> const &fn); // 接受到信息的回调函数

    void set_send_buf(const char *str); // 设定send buf
    Buffer *read_buf();
    Buffer *send_buf();

    RC Read(); // 读操作
    RC Write(); // 写操作
    RC Send(std::string &msg); // 输出信息
    RC Send(const char *msg, int len); // 输出信息
    RC Send(const char *msg);

    Socket *socket() const;

    void OnMessage(); // 当接收到信息时，进行回调
    void OnClose(); // 关闭时，进行回调

private:

    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;

    ConnectionState state_;

    std::unique_ptr<Buffer> read_buf_;
    std::unique_ptr<Buffer> send_buf_;

    std::function<void(int)> on_close_;
    std::function<void(TcpConnection *)> on_message_;

    RC ReadNonBlocking();
    RC WriteNonBlocking();
};
```

`TcpServer`是对整个服务器的管理，他通过创建`acceptor`来接收连接。

```c++
class TcpServer
{
    public:
    DISALLOW_COPY_AND_MOVE(TcpServer);
    TcpServer();
    TcpServer(const char *ip, const int port);
    ~TcpServer();

    void Start();

    void set_connection_callback(std::function < void(TcpConnection *)> const &fn);
    void set_message_callback(std::function < void(TcpConnection *)> const &fn);

    RC HandleClose(int fd);
    RC HandleNewConnection(int fd);

    private:
        std::unique_ptr<EventLoop> main_reactor_;
        std::unique_ptr<Acceptor> acceptor_;
        std::unordered_map<int, std::unique_ptr<TcpConnection>> connectionsMap_;
		//std::unordered_map<int, TcpConnection *> connectionsMap_;
        std::vector<std::unique_ptr<EventLoop>> sub_reactors_;

        std::unique_ptr<ThreadPool> thread_pool_;

        std::function<void(TcpConnection *)> on_connect_;
        std::function<void(TcpConnection *)> on_message_;
};
```

