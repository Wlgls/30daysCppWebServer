#pragma once
#include "common.h"
#include <memory>
#include <mutex>
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

    void DoToDoList();
    void QueueOneFunc(std::function<void()> fn);

private:
    std::unique_ptr<Epoller> poller_;
    std::vector<std::function<void()>> to_do_list_;
    std::mutex mutex_;

};