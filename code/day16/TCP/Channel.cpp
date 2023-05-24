

#include "Channel.h"

#include <unistd.h>
#include <utility>
#include "EventLoop.h"
#include "Socket.h"
#include "common.h"

const short Channel::READ_EVENT = 1;
const short Channel::WRITE_EVENT = 2;
const short Channel::ET = 4;

Channel::Channel(int fd, EventLoop *loop) : fd_(fd), loop_(loop),
                                            listen_events_(0), ready_events_(0),
                                            in_epoll_(false){};

Channel::~Channel() { 
    loop_->DeleteChannel(this); 
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
    if(ready_events_ & READ_EVENT){
        read_callback_();
    }
    if(ready_events_ & WRITE_EVENT){
        write_callback_();
    }
}

void Channel::EnableRead(){
    listen_events_ |= READ_EVENT;
    loop_->UpdateChannel(this);
}

void Channel::EnableWrite(){
    listen_events_ |= WRITE_EVENT;
    loop_->UpdateChannel(this);
}

void Channel::EnableET(){
    listen_events_ |= ET;
    loop_->UpdateChannel(this);
}

int Channel::fd() const { return fd_; }

short Channel::listen_events() const { return listen_events_; }
short Channel::ready_events() const { return ready_events_; }

bool Channel::IsInEpoll() const { return in_epoll_; }
void Channel::SetInEpoll(bool in) { in_epoll_ = in; }

void Channel::SetReadyEvents(short ev){
    if( ev & READ_EVENT){
        ready_events_ |= READ_EVENT;
    }
    if( ev & WRITE_EVENT ){
        ready_events_ |= WRITE_EVENT;
    }
    if(ev & ET){
        ready_events_ |= ET;
    }
}

void Channel::set_read_callback(std::function<void()> const &callback) { read_callback_ = std::move(callback); }
void Channel::set_write_callback(std::function<void()> const &callback) { write_callback_ = std::move(callback); }