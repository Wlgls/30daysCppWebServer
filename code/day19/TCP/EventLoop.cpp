#include "EventLoop.h"
#include <vector>
#include "Channel.h"
#include "Epoller.h"
#include "common.h"
#include "stdio.h"

EventLoop::EventLoop() { poller_ = std::make_unique<Epoller>(); }

EventLoop::~EventLoop() {}

void EventLoop::Loop(){
    while(true){
        for (Channel *active_ch : poller_->Poll()){
            active_ch->HandleEvent();
        }
        DoToDoList();
    }
}

void EventLoop::UpdateChannel(Channel *ch) { poller_->UpdateChannel(ch); }
void EventLoop::DeleteChannel(Channel *ch) { poller_->DeleteChannel(ch); }

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