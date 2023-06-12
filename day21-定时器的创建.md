# day21-定时器的创建

为了在特定时间执行特定任务，定时器在服务器上扮演着非常重要的功能。使用`TCP`长连接需要客户端定时向服务端发送心跳请求。或者在特定时间执行特定任务，比如定点秒杀等任务。

为了实现定时器功能，首先需要对时间进行保存，因此定义一个`TimeStamp`类用于保存相应的时间
```c++
class TimeStamp{
    public:
        TimeStamp();
        explicit TimeStamp(int64_t micro_seconds);
    private:
        int64_t micro_seconds_;
}
```

此外，我们还需要获取当前的时间，而在实际的应用场景中，我们往往存储的是未来的某个**绝对时间**，以抵达这个时间时进行相应的操作，为了定义了两个函数，并将其设定为静态成员函数，并使用`inline`关键字解决频繁调用他们引起大量消耗栈空间的问题。

我们使用`gettimeofday`来获取当前的时间，时间精度达到了1微秒，基本满足了本项目的需求。

```c++
class TimeStamp{
    public:
        static TimeStamp Now();
        static TimeStamp AddTime(TimeStamp timestamp, double add_seconds);
}

inline TimeStamp TimeStamp::Now(){
    struct timeval time;
    gettimeofday(&time, NULL);
    return TimeStamp(time.tv_sec * kMicrosecond2Second + time.tv_usec);
};

inline TimeStamp TimeStamp::AddTime(TimeStamp timestamp, double add_seconds){
    int64_t add_microseconds = static_cast<int64_t>(add_seconds) * kMicrosecond2Second;   
    return TimeStamp(timestamp.microseconds() + add_microseconds);
};
```

为了方便进行比较，还对重载关系运算符进行了重载。

对`TimeStamp`进行定义后，我们需要定义一个`Timer`，其成员变量会包含一个`TimeStamp`类型的成员和一个回调任务`callback`。当达到某个时间戳时，则调用回调函数。

在我们创建`Timer`对象时，我们需要传入一个T`TimeStamp`和一个`callback`以确定在抵达某个时间时，回调函数会被触发。在某些任务中，我们还需要重复的调用我们的回调函数，因此需要一个额外的参数用于保证触发的间隔时间。

```c++
Timer::Timer(TimeStamp timestamp, std::function<void()>const &cb, double interval = 0.0)
    : expiration_(timestamp),
      callback_(std::move(cb)),
      interval_(interval),
      repeat_(interval > 0.0){};
      
class Timer
{
    private:
        TimeStamp expiration_; // 定时器的绝对时间
        std::function<void()> callback_; // 到达时间后进行回调
        double interval_; // 如果重复，则重复间隔
        bool repeat_;
}
```

在重复任务中，`Timer`中的`expiration_`需要经常更新，暂且使用与muduo相同的管理方法，也就是使用普通成员变量进行管理。

在`Timer`中，只需要完成两个任务，第一个当抵达时间时运行回调函数，第二个就是当需要进行重复触发时，需要对`expiration_`进行更新。

```c++
void Timer::ReStart(TimeStamp now){
    expiration_ = TimeStamp::AddTime(now, interval_);
}

void Timer::run() const{
    callback_();
}
```

`TimeStamp`和`Timer`只是作为定时器的最基础的组件。对于定时器的具体的实现，muduo中采用了`c++`中`set`数据结构对`Timer`进行存储，并以时间戳进行排序。以方便获取所有超过当前时间的定时任务。

那么如何获得当前超时的定时任务呢。`muduo`中采用`timerfd`将定时计时的任务交给了系统，由系统负责，当到达时间后，`timerfd`变成了可读，这样就可以将其加入到`Epoll`中由`Epoll`监控。

```c++
TimerQueue::TimerQueue(EventLoop *loop)
    : loop_(loop)
{
    CreateTimerfd();
    channel_ = std::make_unique<Channel>(timerfd_, loop_);
    channel_->set_read_callback(std::bind(&TimerQueue::HandleRead, this));
    channel_->EnableRead();
}
```

当超时时`timerfd`会变得可读，此时调用`HandleRead`操作。
```c++
void TimerQueue::HandleRead(){
    ReadTimerFd(); // 将事件读出来，防止loop陷入忙碌状态
    active_timers_.clear(); 
    
    auto end = timers_.lower_bound(Entry(TimeStamp::Now(), reinterpret_cast<Timer *>(UINTPTR_MAX)));
    active_timers_.insert(active_timers_.end(), timers_.begin(), end); // 将所有超时事件放入到激活序列中
    timers_.erase(timers_.begin(), end);

    for (const auto &entry : active_timers_)
    {
        entry.second->run();
    }
    ResetTimers(); // 对于部分事件，可能存在重复属性，需要将其重新加入到定时器队列中。
}

void TimerQueue::ResetTimers() {
    for (auto& entry: active_timers_) {
        if ((entry.second)->repeat()) {
            auto timer = entry.second;
            timer->ReStart(TimeStamp::Now());
            Insert(timer);
        } else {
            delete entry.second;
        }
    } 

    if (!timers_.empty()) {
        ResetTimerFd(timers_.begin()->second);
    }
}
```

当我们向定时器加入一个定时任务时，我们需要判断是否将该任务加入到了队首，如果加入到了队首，那么就需要更新`timerfd`的状态，设置一个新的超时时间。

```c++
void TimerQueue::AddTimer(TimeStamp timestamp, std::function<void()> const &cb, double interval){
    Timer * timer = new Timer(timestamp, cb, interval);

    if (Insert(timer))
    {
        ResetTimerFd(timer);
    }
}

bool TimerQueue::Insert(Timer * timer){
    bool reset_instantly = false;
    if(timers_.empty() || timer->expiration() < timers_.begin()->first){
        reset_instantly = true;
    }
    timers_.emplace(std::move(Entry(timer->expiration(), timer)));
    return reset_instantl;
}

void TimerQueue::ResetTimerFd(Timer *timer){
    struct itimerspec new_;
    struct itimerspec old_;
    memset(&new_, '\0', sizeof(new_));
    memset(&old_, '\0', sizeof(old_));

    int64_t micro_seconds_dif = timer->expiration().microseconds() - TimeStamp::Now().microseconds();
    if (micro_seconds_dif < 100){
        micro_seconds_dif = 100;
    }

    new_.it_value.tv_sec = static_cast<time_t>(
        micro_seconds_dif / kMicrosecond2Second);
    new_.it_value.tv_nsec = static_cast<long>((
        micro_seconds_dif % kMicrosecond2Second) * 1000);
    // 获取监控的事件的时间距离当前有多久。

    int ret = ::timerfd_settime(timerfd_, 0, &new_, &old_);
}
```

至此，一个基础的定时器任务就完成了，我们在`EventLoop`保存一个成员变量, 并创建三个回调函数以方便在最外层定义相应的定时任务。
```c++
class EventLoop
{
public: 
    void RunAt(TimeStamp timestamp, std::function<void()> const & cb);
    void RunAfter(double wait_time, std::function < void()>const & cb);
    void RunEvery(double interval, std::function<void()> const & cb);
private:
    std::unique_ptr<TimerQueue> timer_queue_;
};
```

此外，在过去的操作中，我们的`main_reactor_`由`TcpServer`来管理，这过于底层了，无法额外的为其定义任务，如定时器功能。因此，将`main_reactor_`移交给最外层来管理，从而更好的自定义功能。在此基础上，在`HttpServer`定义了一些函数用于测试。

运行`./test/WebServer`以观察程序的运行效果。