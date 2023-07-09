#include "EventLoop.h"

#include <vector>

#include "Channel.h"
#include "Epoll.h"

EventLoop::EventLoop() { epoll_ = new Epoll(); }

EventLoop::~EventLoop() { Quit(), delete epoll_; }

void EventLoop::Loop() {
    while (!quit_) {
        std::vector<Channel *> chs;
        chs = epoll_->Poll();
        for (auto &ch : chs) {
            ch->HandleEvent();
        }
        
    }
}

void EventLoop::Quit() { quit_ = true; }

void EventLoop::UpdateChannel(Channel *ch) { epoll_->UpdateChannel(ch); }
void EventLoop::DeleteChannel(Channel *ch) { epoll_->DeleteChannel(ch); }
