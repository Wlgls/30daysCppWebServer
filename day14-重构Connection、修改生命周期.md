# day14-重构TcpConnection、修改生命周期

* 首先，在debug过程中，对内容在此进行了精简，去掉了一些无效的代码，例如将`Socket`类去掉了，这是因为`Socket`类定义的操作一般只由`Accpetor`来调用，直接将其封装在`Acceptor`中更容易让人理解。

* 本章内容偏多，主要是为了理清程序运行的逻辑时，对代码进行了大幅度的更改。

在昨天的重构中，将每个类独属的资源使用`unique_ptr`进行了包装。但是对于`TcpConnection`这个类，其生命周期模糊，使用`unique_ptr`很容易导致内存泄漏。这是因为我们的对于`TcpConnection`是被动关闭，当我们`channel`在`handleEvent`时，发现客户端传入了关闭连接的信息后，直接对`onClose`进行了调用。因此如果使用`unqiue_ptr`时，我们在调用`onclose`时已经销毁了`tcpconnection`，而对应的`channel`也会被移除，但是此时的`HandleEvent`并没有结束，因此存在了内存泄漏。

* 针对，这个问题，总要从三个步骤进行。
  1. 使用`shared_ptr`智能指针管理`TcpConnection`。
  2. 在`HandleEvent`和销毁时，增加引用计数。
  3. 将`HandleClose`操作移交给`main_reactor_`进行。

### 使用`shared_ptr`对`TcpConnection`进行管理

为了解决这个问题，我们采用`shared_ptr`对`TcpConnection`进行了管理。这样就方便延长`TcpConnection`的生命周期。

具体的应用并不赘述，将`TcpConnection`继承自`enable_shared_from_this`即可使用`shared_ptr`管理。并在`TcpServer`中使用`shared_ptr`保存`TcpConnection`。
```c++
// TcpConnection.h
class TcpConnection : public std::enable_shared_from_this<TcpConnection>{
    //...
}

// TcpServer.h
class TcpServer{
    private:
        std::map<int, std::shared_ptr<TcpConnection>> connectionsMap_;
}
```

### 增加`TcpConnection`的引用计数

在当前状态下，在创建`TcpConnection`会将其加入到`connectionsMap_`使其引用计数变成了`1`，之后当`TcpConnection`处理`HandleEvent`受到关闭信号时，会直接调用`HandleClose`，这时会将`TcpConnection`从`connectionsMaps_`释放，引用计数变成`0`，直接销毁，但是`HandleEvent`并没有处理结束，从而导致了内存泄漏。

为了解决该问题，进行了两点处理。
1. 在`HandleEvent`时，增加了引用计数。

具体的，在`Channel`处增加一个指向`TcpConnection`的`weak_ptr`，当进行`HandleEvent`时，增加其应用计数。

```c++
// Channel.h
class Channel{
public:
    void HandleEvent() const;
    void HandleEventWithGuard() const;
    void Tie(const std::shared_ptr<void> &ptr);
private:
    std::weak_ptr<void> tie_;
}
//Channel.cpp
void Channel::HandleEvent() const{
    if(tie_){
        std::shared_ptr<void> guard() = tie_.lock();
        HandleEventWithGuard();
    }else{
        HandleEventWithGuard();
    }
}
void Channel::HandleEventWithGuard() const{
    if(ready_events_ & READ_EVENT){
        read_callback_();
    }
    if(ready_events_ & WRITE_EVENT){
        write_callback_();
    }
}
```

当我们建立`TcpConnection`时，会首先将其绑定在`Channel`的`tie_`上，随后，在令其进行监听读操作，这样就可以保证`Channel`在`HandleEvent`时，会增加`TcpConnection`的引用计数。
```c++
// TcpConnection.cpp
void TcpConnection::ConnectionEstablished(){
    state_ = ConnectionState::Connected;
    channel_->Tie(shared_from_this());
    channel_->EnableRead();
    if (on_connect_){
        on_connect_(shared_from_this());
    }
}

// TcpServer.cpp
inline void TcpServer::HandleNewConnection(int fd){
    assert(fd != -1);
    uint64_t random = fd % sub_reactors_.size();
    
    // 创建TcpConnection对象
    std::shared_ptr<TcpConnection> conn = std::make_shared<TcpConnection>(sub_reactors_[random].get(), fd, next_conn_id_);
    std::function<void(const std::shared_ptr<TcpConnection> &)> cb = std::bind(&TcpServer::HandleClose, this, std::placeholders::_1);
    conn->set_connection_callback(on_connect_);
    conn->set_close_callback(cb);
    conn->set_message_callback(on_message_);

    connectionsMap_[fd] = conn;
    // 分配id
    ++next_conn_id_;
    if(next_conn_id_ == 1000){
        next_conn_id_ = 1;
    }

    // 将connection分配给Channel的tie,增加计数 并开始监听读事件
    conn->ConnectionEstablished();
}
```

这样就保证了只有当`HandleEvent`结束后，`TcpConnection`的引用计数才会变成`0`。

2. 在销毁时，`HandleEvent`结束后，增加引用计数。

上述操作主要是在`HandleEvent`进行时，增加了`TcpConnection`的引用计数。在`HandleEvent`之后增加引用计数可以使程序更加鲁棒。

具体的，我们在`EventLoop`处，增加一个`to_do_list_`列表，并在每次`TcpConnection`的销毁时，向`to_do_list_`处增加一个`TcpConnection`销毁程序从而增加`TcpConnection`的计数，这个列表中的任务只有在`HandleEvent`之运行，这样就保证了`TcpConnection`的销毁，必然时在`HandleEvent`之后的。

```c++
void EventLoop::Loop(){
    while(true){
        for (Channel *active_ch : poller_->Poll()){
            active_ch->HandleEvent();
        }
        DoToDoList();
    }
}

void EventLoop::DoToDoList(){
    // 此时已经epoll_wait出来，可能存在阻塞在epoll_wait的可能性。
    std::vector < std::function<void()>> functors;
    {
        // 加锁 保证线程同步
        std::unique_lock<std::mutex> lock(mutex_); 
        functors.swap(to_do_list_);
    }
    for(const auto& func: functors){
        func();
    }
}
```

在`HandleClose`时，会将`TcpConnection`的`ConnectionDestructor`加入到`to_do_list_`中.
```cpp
// TcpServer.cpp
inline void TcpServer::HandleCloseInLoop(const std::shared_ptr<TcpConnection> & conn){

    auto it = connectionsMap_.find(conn->fd());
    assert(it != connectionsMap_.end());
    connectionsMap_.erase(connectionsMap_.find(conn->fd()));

    EventLoop *loop = conn->loop();
    loop->QueueOneFunc(std::bind(&TcpConnection::ConnectionDestructor, conn));
}

// EventLoop.cpp
void EventLoop::QueueOneFunc(std::function<void()> cb){
    {
        // 加锁，保证线程同步
        std::unique_lock<std::mutex> lock(mutex_);
        to_do_list_.emplace_back(std::move(cb));
    }
}
```

当前的版本对`TcpConnection`的生命周期管理已经差不多是安全的了。在连接到来时，创建`TcpConnection`,并用`shared_ptr`管理，加入到`ConnectionsMap_`中，此时引用计数为1.

而当`HandleEvent`时，使用`weak_ptr.lock`增加`TcpConnection`的应用计数，引用计数为2。

当关闭时，`TcpServer`中`erase`后`TcpConnection`引用计数变为了1。之后将再次当前连接的销毁程序加入到`DoToDoList`使引用计数变为2, 而`HandleEvent`之后完之后引用计数变为了1。当`DoToDoList`执行完成之后，引用计数变成了0。便自动销毁。

### 将从`ConnectionsMaps_`释放`TcpConnection`的操作移交给`main_reactor_`

考虑这样一个问题，当同时有多个连接来连接时，而正好某个连接正好在关闭时，程序会发生什么。

当`sub_reactor_`在处理`HandleEvent`时，接收到关闭请求，此时其会调用`HandleClose`，在这个函数中，会有一个`connectionsMap_.erase()`操作。但是此时`main_reactor_`可能正在接收连接会向`connectionsMap_`中添加连接。由于`connectionsMap_`底层是红黑树，并不支持同时读写操作。因此这是线程冲突的。

因此对于此操作有两种可能，一个是加锁，另一个就是将`HandleClose`的中的`connectionsMap_.erase`操作移交给`main_reactor_`来操作。

在这里实现了第二种操作，为了实现这种操作，必须要获得当前运行线程的`id`，并判断其与对应`reactor_`的线程`id`是否相同。

我们使用定义了`CurrenntThread`来获取当前运行线程的线程id.

由于`EventLoop`的创建是在主线程中，只是将`EventLoop::Loop`分配给了不同的子线程，因此在`Loop`函数中调用`get_id()`并将其保存在`EventLoop`的成员变量中。

```c++
// EventLoop.cpp
void EventLoop::Loop(){
    // 将Loop函数分配给了不同的线程，获取执行该函数的线程
    tid_ = CurrentThread::tid();
    while (true)
    {
        for (Channel *active_ch : poller_->Poll()){
            active_ch->HandleEvent();
        }
        DoToDoList();
    }
}
```

当我们判断当前运行的线程是否是`EventLoop`对应的线程，只需要比较`tid_`即可。
```c++
bool EventLoop::IsInLoopThread(){
    return CurrentThread::tid() == tid_;
}
```

通过以上操作，我们就可以将其保证`connectionsMap_.erase`操作由`main_reactor_`线程进行操作。

具体的，我们对`HandleClose`进行一层额外的封装。当调用`HandleClose`时，会判断调用该函数的线程是否是`main_reactor_`对应的线程,如果是，就直接运行，如果不是，则加入`main_reactor_`的`to_do_list_`中，由`main_reactor_`后续进行操作。

```c++
// TcpServer.cpp
inline void TcpServer::HandleClose(const std::shared_ptr<TcpConnection> & conn){
    std::cout <<  CurrentThread::tid() << " TcpServer::HandleClose"  << std::endl;
    main_reactor_->RunOneFunc(std::bind(&TcpServer::HandleCloseInLoop, this, conn));
}

inline void TcpServer::HandleCloseInLoop(const std::shared_ptr<TcpConnection> & conn){
    std::cout << CurrentThread::tid() << " TcpServer::HandleCloseInLoop - Remove connection id: " <<  conn->id() << " and fd: " << conn->fd() << std::endl;
    auto it = connectionsMap_.find(conn->fd());
    assert(it != connectionsMap_.end());
    connectionsMap_.erase(connectionsMap_.find(conn->fd()));

    EventLoop *loop = conn->loop();
    loop->QueueOneFunc(std::bind(&TcpConnection::ConnectionDestructor, conn));
}

// EventLoop.cpp
void EventLoop::RunOneFunc(std::function<void()> cb){
    if(IsInLoopThread()){
        cb();
    }else{
        QueueOneFunc(cb);
    }
}
```

但是上述仍然存在一个比较严重的问题，由于`to_do_list_`只有在`HandleEvent`之后进行处理，如果当前`Epoller`监听的没有事件发生，那么就会堵塞在`epoll_wait`处，这对于服务器的性能影响是灾难性的。为此，我们希望在将任务加入`to_do_list_`时，唤醒相应的`Epoller`。

为了实现该操作，在`EventLoop`处，增加了一个`wakeup_channel_`，并对其进行监听读操作。当我们为`to_do_list_`添加任务时，如果如果不是当前线程，就随便往`wakeup_channel_`对应的`fd`写点东西，此时，读事件会监听到，就不会再阻塞在epoll_wait中了，并可以迅速执行`HandleCloseInLoop`操作，释放`TcpConnection`。

```cpp
// EventLoop.cpp
EventLoop::EventLoop() : tid_(CurrentThread::tid()) { 
    poller_ = std::make_unique<Epoller>();
    wakeup_fd_ = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    wakeup_channel_ = std::make_unique<Channel>(wakeup_fd_, this);
    calling_functors_ = false;

    wakeup_channel_->set_read_callback(std::bind(&EventLoop::HandleRead, this));
    wakeup_channel_->EnableRead();
}


void EventLoop::QueueOneFunc(std::function<void()> cb){
    {
        // 加锁，保证线程同步
        std::unique_lock<std::mutex> lock(mutex_);
        to_do_list_.emplace_back(std::move(cb));
    }

    // 如果调用当前函数的并不是当前当前EventLoop对应的的线程，将其唤醒。主要用于关闭TcpConnection
    // 由于关闭连接是由对应`TcpConnection`所发起的，但是关闭连接的操作应该由main_reactor所进行(为了释放ConnectionMap的所持有的TcpConnection)
    if (!IsInLoopThread() || calling_functors_) {
        uint64_t write_one_byte = 1;  
        ssize_t write_size = ::write(wakeup_fd_, &write_one_byte, sizeof(write_one_byte));
        (void) write_size;
        assert(write_size == sizeof(write_one_byte));
    } 
}


void EventLoop::HandleRead(){
    // 用于唤醒EventLoop
    uint64_t read_one_byte = 1;
    ssize_t read_size = ::read(wakeup_fd_, &read_one_byte, sizeof(read_one_byte));
    (void) read_size;
    assert(read_size == sizeof(read_one_byte));
    return;
}

```

