

#pragma once
#include "mutex.h"
#include "common.h"
#include <memory>
#include <functional>
class TimerQueue;
class TimeStamp;
class Epoller;
class EventLoop
{
public:


    DISALLOW_COPY_AND_MOVE(EventLoop);
    EventLoop();
    ~EventLoop();
    
    void RunAt(TimeStamp timestamp, std::function<void()> const & cb);
    void RunAfter(double wait_time, std::function < void()>const & cb);
    void RunEvery(double interval, std::function<void()> const & cb);
    

    void Loop();
    void UpdateChannel(Channel *ch);
    void DeleteChannel(Channel *ch);

    void DoToDoList();
    void QueueOneFunc(std::function<void()> fn);
    

private:
    std::unique_ptr<Epoller> poller_;
    std::vector<std::function<void()>> to_do_list_;
    MutexLock mutex_;
    std::unique_ptr<TimerQueue> timer_queue_;
};