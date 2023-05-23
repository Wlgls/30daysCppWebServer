
/*

作为Reactor模式的服务器。

将整个服务器抽象成一个Server类，这个类中有一个main_reactor，

里面的核心是EventLoop。是一个事件循环。其主要作用就是监听epoll中上的事件，并对不同事件类型进行不同的处理。

*/

#pragma once

#include "Epoller.h"
#include "common.h"
#include <memory>
#include <functional>

class Epoller;
class EventLoop
{
public:
    DISALLOW_COPY_AND_MOVE(EventLoop);
    EventLoop();
    ~EventLoop();

    void Loop() const;
    void UpdateChannel(Channel *ch) const;
    void DeleteChannel(Channel *ch) const;

    void DoToDoList();
    void QueueOneFunc(std::function<void()> fn);

private:
    std::unique_ptr<Epoller> poller_;
};