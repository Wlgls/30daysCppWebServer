#include "EventLoop.h"
#include <vector>
#include "Channel.h"
#include "Epoller.h"
#include <memory>
#include "common.h"
#include <stdio.h>
#include "mutex.h"
#include "timestamp.h"
#include "timer.h"
#include "timerqueue.h"

EventLoop::EventLoop() { 
    poller_ = std::make_unique<Epoller>();
    timer_queue_ = std::make_unique<TimerQueue>(this);
}

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

void EventLoop::RunAt(TimeStamp timestamp, std::function<void()>const & cb){
    timer_queue_->AddTimer(timestamp, std::move(cb), 0.0);
}

void EventLoop::RunAfter(double wait_time, std::function<void()> const & cb){
    TimeStamp timestamp(TimeStamp::AddTime(TimeStamp::Now(), wait_time));
    timer_queue_->AddTimer(timestamp, std::move(cb), 0.0);
}

void EventLoop::RunEvery(double interval, std::function<void()> const & cb){
    TimeStamp timestamp(TimeStamp::AddTime(TimeStamp::Now(), interval));
    timer_queue_->AddTimer(timestamp, std::move(cb), interval);
}

