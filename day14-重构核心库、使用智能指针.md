# day13-重构核心库、使用智能指针

* 在断开连接时，会发生内存泄漏。

之前的操作已经对完成了一个分离业务逻辑的简单网络库，但是整个项目越来越复杂，模块越来越多。为了弥补之前的设计、细节缺陷。应该对程序进行重构和梳理，以方便自己进一步的去处理和实现功能。

本次重构主要包含以下几个方面：

* 更换了部分函数名。减少无用的函数，使代码看起来更简洁，可读。
* 进行内存管理。在之前的操作所有的内存都是由裸指针进行处理的，在类的构造阶段分配内存，析构释放内存，为了更加方便的管理内存，对于类自己拥有的资源使用了智能指针`std::unique_ptr<>`进行管理，对于不属于自己的资源，但是借用的资源，使用裸指针进行处理。
* 避免资源的复制操作 ，尽量使用移动语义进行所有权转移，以提升程序性能。

### 关于common.h

对于大部分的类，我们都不希望实现其拷贝构造函数，移动构造函数和赋值运算符，简单的操作可以在每一个类中使用`=delete`来保证其不被编译器自动实现。但是如果每个类都这么写，显然不够清晰且冗余，因此采用了宏来实现。
```cpp
// common.h
#define DISALLOW_COPY(cname)     \
  cname(const cname &) = delete; \
  cname &operator=(const cname &) = delete;

#define DISALLOW_MOVE(cname) \
  cname(cname &&) = delete;  \
  cname &operator=(cname &&) = delete;

#define DISALLOW_COPY_AND_MOVE(cname) \
  DISALLOW_COPY(cname);               \
  DISALLOW_MOVE(cname);
```

### 关于Socket类

Socket类主要是对socket操作进行了封装，并主要应用在`Acceptor`类中和`Connection`类中，但是非常明显的可以发现，在`Connection`中，`Socket`成员变量并没有发挥太大的作用，只是单纯的获取`Socket`中的文件描述符，因此，对于`Connection`类其成员变量在一定程度上是冗余的。因此完全可以删掉`Socket`类，并将相应的操作直接封装在`Acceptor`中。

### 关于Channel类

`Channel`类是网络库的核心组建之一，他对`socket`进行了更深度的封装，保存了我们需要对`socket`监听的事件，当前`socket`已经准备好的事件并进行处理。此外，为了更新和获取在`epoller`中的状态，需要使用`EventLoop`进行管理，由于只是使用`EventLoop`，因此采用裸指针进行内存管理。

```cpp
class Channel {
    public:
        DISALLOW_COPY_AND_MOVE(Channel);
        Channel(int fd, EventLoop * loop);
        
        ~Channel();

        void HandleEvent() const; // 处理事件
        void EnableRead();  // 允许读
        void EnableWrite(); // 允许写
        void EnableET(); // 以ET形式触发
        void DisableWrite();

        int fd() const;  // 获取fd
        short listen_events() const; // 监听的事件
        short ready_events() const; // 准备好的事件

        bool IsInEpoll() const; // 判断当前channel是否在poller中
        void SetInEpoll(bool in = true); // 设置当前状态为poller中
        

        void SetReadyEvents(int ev);
        void set_read_callback(std::function<void()> const &callback);// 设置回调函数
        void set_write_callback(std::function<void()> const &callback);

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

### 针对Epoller类

主要是进行IO多路复用，保证高并发。在Epoller类主要是对epoll中channel的监听与处理。
```cpp
class Epoller
{
public:
    DISALLOW_COPY_AND_MOVE(Epoller);

    Epoller();
    ~Epoller();

    // 更新监听的channel
    void UpdateChannel(Channel *ch) const;
    // 删除监听的通道
    void DeleteChannel(Channel *ch) const;

    // 返回调用完epoll_wait的通道事件
    std::vector<Channel *> Poll(long timeout = -1) const;

    private:
        int fd_;
        struct epoll_event *events_;
};
```

### 针对EventLoop类

该类是对事件的轮询和处理。由于每一个EventLoop主要是不断地调用`epoll_wait`来获取激活的事件，并处理。这也就意味着`Epoll`是独属于`EventLoop`的成员变量，随着`EventLoop`的析构而析构，因此可以采用智能指针
```cpp
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

### 针对Acceptor类

Acceptor主要用于服务器接收连接，并在接受连接之后进行相应的处理。这个类需要独属于自己的Channel，因此采用了智能指针管理。并且将socket相应的操作也直接封装在了`Accptor`中，并且为了方便自定义ip地址和port端口，不直接将ip和port绑死，而是通过传参的方式。
```cpp
// Acceptor.h
class Acceptor{
    public:
        DISALLOW_COPY_AND_MOVE(Acceptor);
        Acceptor(EventLoop *loop, const char * ip, const int port);
        ~Acceptor();

        void set_newconnection_callback(std::function<void(int)> const &callback);
        
        // 创建socket
        void Create();

        // 与ip地址绑定
        void Bind(const char *ip, const int port);
        
        // 监听Socket
        void Listen();

        // 接收连接
        void AcceptConnection();

    private:
        EventLoop *loop_;
        int listenfd_;
        std::unique_ptr<Channel> channel_;
        std::function<void(int)> new_connection_callback_;
};

// Acceptor.cpp
Acceptor::Acceptor(EventLoop *loop, const char * ip, const int port) :loop_(loop), listenfd_(-1){
    Create();
    Bind(ip, port);
    Listen();
    channel_ = std::make_unique<Channel>(listenfd_, loop);
    std::function<void()> cb = std::bind(&Acceptor::AcceptConnection, this);
    channel_->set_read_callback(cb);
    channel_->EnableRead();
}
```

### 针对TcpConnection类

对于每个TCP连接，都可以使用一个类进行管理，在这个类中，将注意力转移到对客户端socket的读写上，除此之外，他还需要绑定几个回调函数，例如当接收到信息时，或者需要关闭时进行的操作。
并且增加了一个conn_id的成员变量，主要是`fd`可能被复用，在debug时，可以更清晰的追寻问题。
```cpp
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
```

### 关于TcpServer类

`TcpServer`是对整个服务器的管理，他通过创建`acceptor`来接收连接。并管理`TcpConnection`的添加。

在这个类中，由于`TcpConnection`的生命周期模糊，暂时使用了裸指针，后续将会改造成智能指针。
```cpp
// TcpServer.h
class TcpServer
{
    public:
    DISALLOW_COPY_AND_MOVE(TcpServer);
    TcpServer(const char *ip, const int port);
    ~TcpServer();

    void Start();

    void set_connection_callback(std::function < void(TcpConnection *)> const &fn);
    void set_message_callback(std::function < void(TcpConnection *)> const &fn);

    void HandleClose(int fd);
    void HandleNewConnection(int fd);

    private:
        std::unique_ptr<EventLoop> main_reactor_;
        int next_conn_id_;
        std::unique_ptr<Acceptor> acceptor_;
        std::vector<std::unique_ptr<EventLoop>> sub_reactors_;
	    std::unordered_map<int, TcpConnection *> connectionsMap_;
        std::unique_ptr<ThreadPool> thread_pool_;
        std::function<void(TcpConnection *)> on_connect_;
        std::function<void(TcpConnection *)> on_message_;
};

// TcpServer.cpp
void TcpServer::HandleClose(int fd){
    auto it =  connectionsMap_.find(fd);
    assert(it != connectionsMap_.end());
    TcpConnection * conn = connectionsMap_[fd];
    connectionsMap_.erase(fd);
    // 如果析构会导致内存泄漏
    // delete conn;
    // 但是没有析构就不会close，服务端停留在`close_wait`状态，客户端停留在`fin_wait`状态。所以在这里暂时进行了close以先关闭连接
    // 可以尝试着两种操作所带来的问题
    ::close(fd);
    conn = nullptr;
}
```