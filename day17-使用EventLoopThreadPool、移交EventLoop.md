# day17-使用EventLoopThreadPool、移交EventLoop

在之前的操作中， 我们在`main_reactor`中创建了`sub_reactor`，但是这意味着只有在`Loop`时才能够调用`CurrentThread::tid()`获取j对应的线程ID，并将其认定为自身的`sub_reactor`的所属线程ID，这并没有问题，但是条理上不够清晰。

我们期望在将创建相应的线程，之后将`sub_reactor`的任务直接分配给相应的线程，并在初始化直接绑定线程ID，这更方便理解，也更方便将不同的模块进行分离。

此外，对于我们的多线程服务器，线程池其实并不需要`task_queue`，因为每一个线程执行的任务是确定且绑定的，因此也可以使用更简单且便于理解的线程池。

为了将构造`EventLoop`也就是构造`sub_reactor`的任务交给子线程并且我们的主线程能够调用相应的`EventLoop`绑定相应的`TcpConnection`。创建了`EventLoopThread`类，这个类将由主线程进行管理。该类主要是`EventLoop`与线程之间的操作。

当我们创建线程时，子线程将首先创建一个`EventLoop`对象，之后由主线程获取该对象的地址，并执行`Loop`函数。

```c++
void EventLoopThread::ThreadFunc(){
    // 由IO线程创建EventLoop对象
    EventLoop loop;
    {
        // 加锁
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop; // 获取子线程的地址
        cv_.notify_one(); // loop_被创建成功，发送通知，唤醒主线程。
    }

    loop_->Loop(); // 开始循环，直到析构
    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = nullptr;
    }
}
```

而我们的主线程，将创建执行该函数的子线程，并获得由子线程所创建的`EventLoop`的地址。
```c++
EventLoop *EventLoopThread::StartLoop(){
    // 绑定当前线程的所执行的函数，并创建子线程
    // 在这个线程中创建EventLoop.
    thread_ = std::thread(std::bind(&EventLoopThread::ThreadFunc, this));
    EventLoop *loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (loop_ == NULL){
            cv_.wait(lock); // 当IO线程未创建LOOP时，阻塞
        }
        // 将IO线程创建的loop_赋给主线程。
        loop = loop_;
    }
    // 返回创建好的线程。
    return loop;
}
```

构造了`EventLoopThread`之后，`EventLoopThreadPool`就非常简单了，创建相应的`EventLoopThread`对象，并执行`StartLoop`函数，保存创建的`EventLoop`的地址，并在需要时返回即可。


```c++
// EventLoopThreadPool
class EventLoopThreadPool{

    public:
        DISALLOW_COPY_AND_MOVE(EventLoopThreadPool);
        EventLoopThreadPool(EventLoop *loop);
        ~EventLoopThreadPool();

        void SetThreadNums(int thread_nums);

        void start();

        // 获取线程池中的EventLoop
        EventLoop *nextloop();

    private:
        EventLoop *main_reactor_;
        // 保存对应的线程对象
        std::vector<std::unique_ptr<EventLoopThread>> threads_;
        // 保存创建的EventLoop
        std::vector<EventLoop *> loops_;

        int thread_nums_;

        int next_;
};
EventLoopThreadPool::EventLoopThreadPool(EventLoop *loop)
    : main_reactor_(loop),
      thread_nums_(0),
      next_(0){};

EventLoopThreadPool::~EventLoopThreadPool(){}

void EventLoopThreadPool::start(){
    for (int i = 0; i < thread_nums_; ++i){
        // 创建EventLoopThread对象，并保存由子线程创建的EventLoop的地址
        std::unique_ptr<EventLoopThread> ptr = std::make_unique<EventLoopThread>();
        threads_.push_back(std::move(ptr));
        loops_.push_back(threads_.back()->StartLoop());
    }
}

EventLoop *EventLoopThreadPool::nextloop(){
    EventLoop *ret = main_reactor_;
    if (!loop_.empty()){
        ret = loops_[next_++];
        // 采用轮询法调度。
        if (next_ == static_cast<int>(loops_.size())){
            next_ = 0;
        }
    }
    return ret;
}
```

而我们只需要在`TcpServer`中创建一个`EventLoopThreadPool`对象，并对这个对象进行操作即可。并在启动服务器时，创建子线程EventLoop。
```c++
// TcpServer.cpp
TcpServer::TcpServer(EventLoop *loop, const char * ip, const int port){
    // ...
    // 创建线程池
    thread_pool_ = std::make_unique<EventLoopThreadPool>(loop);
}


void TcpServer::Start(){
    // 创建子线程和对应的EventLoop
    thread_pool_->start();
    // 主线程启动
    main_reactor_->Loop();
}
```

上述的`EventLoopThreadPool`与之前的`ThreadPool`并没有性能上的提高，但是其将对线程的操作与`TcpServer`分离，将代码进一步的模块化，使可读性大大增强。