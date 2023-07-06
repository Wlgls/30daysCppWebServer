#include "EventLoop.h"

#include "Channel.h"
#include "Epoller.h"
#include "currentthread.h"

#include <memory>
#include <vector>
#include <stdio.h>
#include <pthread.h>

EventLoop::EventLoop() : tid_(CurrentThread::tid()) { 
    poller_ = std::make_unique<Epoller>();
}

EventLoop::~EventLoop() {}

void EventLoop::Loop(){
    // 将Loop函数分配给了不同的线程，获取执行该函数的线程
    tid_ = CurrentThread::tid();
    while (true)
    {
        for (Channel *active_ch : poller_->Poll()){
            active_ch->HandleEvent();
        }
        DoToDoList();
    }
}

void EventLoop::UpdateChannel(Channel *ch) { poller_->UpdateChannel(ch); }
void EventLoop::DeleteChannel(Channel *ch) { poller_->DeleteChannel(ch); }

bool EventLoop::IsInLoopThread(){
    return CurrentThread::tid() == tid_;
}

void EventLoop::RunOneFunc(std::function<void()> cb){
    if(IsInLoopThread()){
        cb();
    }else{
        QueueOneFunc(cb);
    }
}

void EventLoop::QueueOneFunc(std::function<void()> cb){
    {
        // 加锁，保证线程同步
        std::unique_lock<std::mutex> lock(mutex_);
        to_do_list_.emplace_back(std::move(cb));
    }
}

void EventLoop::DoToDoList(){
    std::vector < std::function<void()>> functors;
    {
        // 加锁 保证线程同步
        std::unique_lock<std::mutex> lock(mutex_); 
        functors.swap(to_do_list_);
    }
    for(const auto& func: functors){
        func();
    }
}
