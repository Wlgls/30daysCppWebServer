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
    }
}

void EventLoop::UpdateChannel(Channel *ch) { poller_->UpdateChannel(ch); }
void EventLoop::DeleteChannel(Channel *ch) { poller_->DeleteChannel(ch); }

