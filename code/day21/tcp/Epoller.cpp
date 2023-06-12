#include "Epoller.h"


#include "Channel.h"
#include "common.h"
#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>

#define MAX_EVENTS 1000

Epoller::Epoller(){
    fd_ = epoll_create1(0);
    events_ = new epoll_event[MAX_EVENTS];
    memset(events_, 0, sizeof(*events_) * MAX_EVENTS);
}

Epoller::~Epoller(){
    if(fd_ != -1){
        ::close(fd_);
    }
    delete[] events_; // 为什么把他这个删掉了，最后
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

RC Epoller::UpdateChannel(Channel *ch) const{
    int sockfd = ch->fd();
    struct epoll_event ev {};
    ev.data.ptr = ch;
    ev.events = ch->listen_events();
    if (!ch->IsInEpoll())
    {
        if(epoll_ctl(fd_, EPOLL_CTL_ADD, sockfd, &ev) == -1){
            perror("epoll add error");
            return RC_POLLER_ERROR;
        }
        ch->SetInEpoll(true);
    }
    else
    {
        if(epoll_ctl(fd_, EPOLL_CTL_MOD, sockfd, &ev) == -1){
            perror("epoll modify error");
            return RC_POLLER_ERROR;
        }
    }
    return RC_SUCCESS;
}

RC Epoller::DeleteChannel(Channel *ch) const{
    int sockfd = ch->fd();
    if (epoll_ctl(fd_, EPOLL_CTL_DEL, sockfd, nullptr) == -1){
        perror("epoll delete error");
        return RC_POLLER_ERROR;
    }
    ch->SetInEpoll(false);
    return RC_SUCCESS;
}
