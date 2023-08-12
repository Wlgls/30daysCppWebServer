# day19-定时器的创建和使用

为了在特定时间执行特定任务，定时器在服务器上扮演着非常重要的功能。使用`TCP`长连接需要客户端定时向服务端发送心跳请求。或者在特定时间执行特定任务，比如定点秒杀等任务。

因此，在定时器中，我们需要实现的功能主要包含以下部分：
1. 获取一个时间
2. 保存一个特定时间，并在抵达该时间时执行特定的任务。
3. 由于一个项目中，不可能只存在一个定时任务，因此还需要对定时任务进行管理。

定时器功能中，最基础的就是获取相应的时间，我们封装了`TimeStamp`类，并且使用`gettimeofday`来获取当前的时间，时间精度达到了1微秒，基本满足了本项目的需求。

其定义为，在这个类中基本定义了我们所需要的函数，主要包括对当前时间的获取，已经对时间的比较。:
```c++
class TimeStamp{

    public:
        TimeStamp();
        explicit TimeStamp(int64_t micro_seconds);
        // 重载运算符用于比较
        bool operator<(const TimeStamp &rhs) const;
        bool operator==(const TimeStamp &rhs) const;
        // 获取事件的字符串表示，用于日志库
        std::string ToFormattedString() const;
        int64_t microseconds() const;
        
        // 获取当前时间
        static TimeStamp Now();
        // 获取当前时间+add_seconds后的时间
        static TimeStamp AddTime(TimeStamp timestamp, double add_seconds);
    private:
        int64_t micro_seconds_;
};

// 静态函数
inline TimeStamp TimeStamp::Now(){
    struct timeval time;
    gettimeofday(&time, NULL);
    return TimeStamp(time.tv_sec * kMicrosecond2Second + time.tv_usec);
}

inline TimeStamp TimeStamp::AddTime(TimeStamp timestamp, double add_seconds){
    int64_t add_microseconds = static_cast<int64_t>(add_seconds) * kMicrosecond2Second;   
    return TimeStamp(timestamp.microseconds() + add_microseconds);
}

```

上述只是封装了对时间的获取，而我们实现定时器，还需要保存对应的任务，因此，我们还需要定义一个`Timer`，其内部有两个关键的成员变量时间戳`TimeStamp expiration_`和对应需要执行的函数`std::function<void()> callback_`。

除此之外，我们可能会需要重复的执行某些任务，因此还定义了`double interval_`表示重复的时间间隔。

具体的定义如下：

```c++
class Timer
{
public:
    DISALLOW_COPY_AND_MOVE(Timer);
    Timer(TimeStamp timestamp, std::function<void()>const &cb, double interval);

    TimeStamp expiration() const;

    // 抵达时间，运行
    void run() const;
    
    // 如果是重复任务
    void ReStart(TimeStamp now);
    bool repeat() const;

private:
    TimeStamp expiration_; // 定时器的绝对时间
    std::function<void()> callback_; // 到达时间后进行回调
    double interval_; // 如果重复，则重复间隔
    bool repeat_;
};
```

`TimeStamp`和`Timer`只是作为定时器的最基础的组件。对于多个定时器任务。我们采用了`c++`中`set`数据结构对`Timer`进行存储，并以时间戳进行排序。以方便获取所有超过当前时间的定时任务。

而对于获得当前超时的定时任务。我们采用`timerfd`将定时计时的任务交给了系统，由系统负责，当到达时间后，`timerfd`变成了可读，此时就可以执行相应的定时器任务，这样就可以将其加入到`Epoll`中由`Epoll`监控。

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

当超时时`timerfd`会变得可读，此时调用`HandleRead`操作，获取所有可执行的定时任务，并进行执行。
```c++
void TimerQueue::HandleRead(){
    ReadTimerFd(); // 将事件读出来，防止loop陷入忙碌状态
    active_timers_.clear(); 
    
    auto end = timers_.lower_bound(Entry(TimeStamp::Now(), reinterpret_cast<Timer *>(UINTPTR_MAX)));
    active_timers_.insert(active_timers_.end(), timers_.begin(), end); // 将所有超时事件放入到激活序列中
    timers_.erase(timers_.begin(), end);

    // 执行
    for (const auto &entry : active_timers_)
    {
        entry.second->run();
    }
    ResetTimers(); // 对于部分事件，可能存在重复属性，需要将其重新加入到定时器队列中，并且需要重新设定`timerfd`超时时间
}

void TimerQueue::ResetTimers() {
    // 将带有重复属性的定时任务重新加入到set中
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
        // 重新设定`timerfd`超时时间
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

    // 如果时间比对首要早，就更新。
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


为了对定时器进行测试，在`HttpServer`定义了一些函数进行了简单的测试，更加复杂的操作还需要进一步的探索。

进入`build`，运行`./test/webserver`以观察程序的运行效果。