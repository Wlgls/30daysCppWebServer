#pragma once
#include "common.h"

#include <memory>
#include <mutex>
#include <thread>
#include <functional>
#include <vector>
class Epoller;
class EventLoop
{
public:
    DISALLOW_COPY_AND_MOVE(EventLoop);
    EventLoop();
    ~EventLoop();
    
    void Loop();
    void UpdateChannel(Channel *ch);
    void DeleteChannel(Channel *ch);


    // 运行队列中的任务
    void DoToDoList();

    // 将任务添加到队列中。当loop完成polling后运行
    void QueueOneFunc(std::function<void()> fn); 

    // 如果由创建本Loop的线程调用，则立即执行fn任务
    // 否则，将fn加入到队列中，等待之后运行
    void RunOneFunc(std::function<void()> fn);
    
    // 判断调用该函数的是不是当前的线程，即是不是创建当前Loop的线程。
    bool IsInLoopThread();

    void HandleRead();

private:
    std::unique_ptr<Epoller> poller_;
    std::vector<std::function<void()>> to_do_list_;
    std::mutex mutex_;

    int wakeup_fd_;
    std::unique_ptr<Channel> wakeup_channel_;

    bool calling_functors_;
    pid_t tid_;
};