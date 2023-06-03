

#pragma once

#include "Epoller.h"
#include "common.h"
#include <memory>
#include <functional>
#include "mutex.h"
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

    void DoToDoList();
    void QueueOneFunc(std::function<void()> fn);
    

private:
    std::unique_ptr<Epoller> poller_;
    std::vector<std::function<void()>> to_do_list_;
    MutexLock mutex_;
};