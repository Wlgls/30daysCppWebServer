#include "Epoller.h"
#include "Channel.h"

#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

#define MAX_EVENTS 1000

Epoller::Epoller(){
    fd_ = epoll_create1(0);
    events_ = new epoll_event[MAX_EVENTS];
    memset(events_, 0, sizeof(*events_) * MAX_EVENTS);
}

Epoller::~Epoller(){
    // 关闭socket，并释放events_空间
    if(fd_ != -1){
        ::close(fd_);
    }
    delete[] events_; 
}

std::vector<Channel *> Epoller::Poll(long timeout) const{
    std::vector<Channel *> active_channels;
    int nfds = epoll_wait(fd_, events_, MAX_EVENTS, timeout);
    if(nfds == -1){ perror("epoll wait error"); }
    for (int i = 0; i < nfds; ++i){
        Channel *ch = (Channel *)events_[i].data.ptr;
        int events = events_[i].events;
        ch->SetReadyEvents(events);
        active_channels.emplace_back(ch);
    }
    return active_channels;
}

void Epoller::UpdateChannel(Channel *ch) const{
    int sockfd = ch->fd();
    struct epoll_event ev {};
    ev.data.ptr = ch;
    ev.events = ch->listen_events();
    if (!ch->IsInEpoll()){
        if(epoll_ctl(fd_, EPOLL_CTL_ADD, sockfd, &ev) == -1){
            std::cout << "Epoller::UpdateChannel epoll_ctl_add failed" << std::endl;
        }
        ch->SetInEpoll(true);
    }else{
        if(epoll_ctl(fd_, EPOLL_CTL_MOD, sockfd, &ev) == -1){
            std::cout << "Epoller::UpdateChannel epoll_ctl_mod failed" << std::endl;
        }
    }
}

void Epoller::DeleteChannel(Channel *ch) const{
    int sockfd = ch->fd();
    if (epoll_ctl(fd_, EPOLL_CTL_DEL, sockfd, nullptr) == -1){
        std::cout << "Epoller::UpdateChannel epoll_ctl_del failed" << std::endl;
    }
    ch->SetInEpoll(false);
}