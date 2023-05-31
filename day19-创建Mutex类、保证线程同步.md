day19-创建Mutex类、保证线程同步

尽管到现在已经算是完成了一个简单的HTTP服务器，但是对其进行压测时，仍然存在许多问题，总是内存泄漏。

在之前的版本中，在`Channel`中创建了`Tie`以提升`TcpConnection`的生命周期，保证`Channel`在进行`HandleEvent`之后才将`TcpConnection`的引用计数置为0。从而保证`TcpConnection`释放安全。

但是当客户端数量过多时，总会出先问题，可能是因为上述解决方法并不鲁棒。

`muduo`在处理`TcpConnection`时，除了使用`tie_`，在进行释放时，会在`EventLoop`处额外增加`TcpConnection`的引用计数。从而保证鲁棒。

由于`EventLoop`可能管理着多个客户端，因此，我们使用`to_do_list_`保存不同客户端的销毁程序。并在所有的`HandleEvent`结束之后进行。为了保证线程同步，我们在向`to_do_list_`添加函数和运行他时进行加锁。

首先需要定义一个互斥锁，使用了`muduo`的处理方法，对`pthread_mutex`进行了简单的封装。
```c++
class MutexLock {
public:
    DISALLOW_COPY_AND_MOVE(MutexLock)
    MutexLock() {
        pthread_mutex_init(&mutex_, nullptr);
    }

    ~MutexLock() {
        pthread_mutex_destroy(&mutex_);
    }

    bool Lock() {
        return pthread_mutex_lock(&mutex_) == 0;
    }

    bool Unlock() {
        return pthread_mutex_unlock(&mutex_) == 0; 
    }
 
    pthread_mutex_t* mutex() { return &mutex_; } 

    private:
        pthread_mutex_t mutex_; 
};
```

为了管理锁资源，使用了RAII手法进行了管理，RAII是一种利用对象生命周期来控制程序资源的技术。例如在对象构造时获取资源(互斥锁上锁)，接着控制对资源的访问使之在对象的生命周期内始终有效，最后在对象析构时释放资源(互斥锁解锁)。

```c++
class MutexLockGuard {
    public:
    explicit MutexLockGuard(MutexLock& mutex) : mutex_(mutex) {
        mutex_.Lock();
    }
    ~MutexLockGuard() {
        mutex_.Unlock();
    }

    private:
        MutexLock& mutex_;
};
```

实现了锁的定义，就可以进行对`to_do_list_`的处理。

当我们运行和添加函数时，分别进行加锁从而保证线程安全。
```c++
void EventLoop::QueueOneFunc(std::function<void()> cb){
    {
        MutexLockGuard lock(mutex_);
        to_do_list_.emplace_back(std::move(cb));
    }
}

void EventLoop::DoToDoList(){
    std::vector < std::function<void()>> functors;
    {
        MutexLockGuard lock(mutex_);
        functors.swap(to_do_list_);
    }
    for(const auto& func: functors){
        func();
    }
}
```

而`TcpServer`在关闭连接时，就会向`EventLoop`传入`TcpConnection`的毁灭函数以提升`TcpConnection`的生命周期，并保证`EventLoop`在运行所有`HandleEvent`之后运行`to_do_list_`以进行析构。
```c++
//tcpserver.cpp
RC TcpServer::OnClose(const std::shared_ptr<TcpConnection> & conn){
    auto it = connectionsMap_.find(conn->fd());
    assert(it != connectionsMap_.end());
    connectionsMap_.erase(conn->fd());

    EventLoop *loop = conn->loop();
    loop->QueueOneFunc(std::bind(&TcpConnection::ConnectionDestructor, conn));
    return RC_SUCCESS;
}
```

```c++
// EventLoop.cpp
void EventLoop::Loop(){
    while(true){
        for (Channel *active_ch : poller_->Poll()){
            active_ch->HandleEvent();
        }
        DoToDoList();
    }
}
```


当前的版本对`TcpConnection`的生命周期管理已经差不多是安全的了。但是仍然存在许多问题，包括性能太差，线程安全等等。这需要后续进一步的修改。之后暂时先实现定时器和日志库。这两个组件也更方面我们去debug。
