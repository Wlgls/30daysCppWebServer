

#include "Channel.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <utility>
#include "EventLoop.h"
#include "common.h"
#include <iostream>


Channel::Channel(int fd, EventLoop *loop) : fd_(fd), loop_(loop),
                                            listen_events_(0), ready_events_(0),
                                            in_epoll_(false){};

Channel::~Channel() { 
    // loop_->DeleteChannel(this); 
}

void Channel::HandleEvent() const{
    if(tied_){
        std::shared_ptr<void> guard = tie_.lock();
        HandleEventWithGuard();
    }else{
        HandleEventWithGuard();
    }
}

void Channel::Tie(const std::shared_ptr<void> &ptr){
    tied_ = true;
    tie_ = ptr;
}

void Channel::HandleEventWithGuard() const{
    if (ready_events_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        if (read_callback_) {
        read_callback_();
        }
    } 

    if (ready_events_ & EPOLLOUT) {
        if (write_callback_) {
            write_callback_();
        }
    }
}

void Channel::EnableRead(){
    listen_events_ |= (EPOLLIN | EPOLLPRI);
    loop_->UpdateChannel(this);
}

void Channel::EnableWrite(){
    listen_events_ |= EPOLLOUT;
    loop_->UpdateChannel(this);
}

void Channel::EnableET(){
    listen_events_ |= EPOLLET;
    loop_->UpdateChannel(this);
}

int Channel::fd() const { return fd_; }

short Channel::listen_events() const { return listen_events_; }
short Channel::ready_events() const { return ready_events_; }

bool Channel::IsInEpoll() const { return in_epoll_; }
void Channel::SetInEpoll(bool in) { in_epoll_ = in; }

void Channel::SetReadyEvents(int ev){
    ready_events_ = ev;
}

void Channel::set_read_callback(std::function<void()> const &callback) { read_callback_ = std::move(callback); }
void Channel::set_write_callback(std::function<void()> const &callback) { write_callback_ = std::move(callback); }