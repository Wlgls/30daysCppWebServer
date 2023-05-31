

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

    void Loop();
    void UpdateChannel(Channel *ch);
    void DeleteChannel(Channel *ch);



private:
    std::unique_ptr<Epoller> poller_;
};