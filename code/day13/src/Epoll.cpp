#include "Epoll.h"
#include "util.h"
#include "Socket.h"
#include "Channel.h"
#include <unistd.h>
#include <cstring>

#define MAX_EVENTS 1000

Epoll::Epoll(){
    fd_ = epoll_create1(0);
    ErrorIf(fd_ == -1, "epoll create error");
    events_ = new epoll_event[MAX_EVENTS];

    memset(events_, 0, sizeof(*events_) * MAX_EVENTS);
}

Epoll::~Epoll() {
    if (fd_ != -1) {
        close(fd_);
        fd_ = -1;
    }
    delete[] events_;
}

std::vector<Channel *> Epoll::Poll(int timeout) {
    std::vector<Channel *> active_channels;
    int nfds = epoll_wait(fd_, events_, MAX_EVENTS, timeout);
    ErrorIf(nfds == -1, "epoll wait error");
    for (int i = 0; i < nfds; ++i) {
        Channel *ch = (Channel *)events_[i].data.ptr;
        int events = events_[i].events;
        if (events & EPOLLIN) {
        ch->SetReadyEvents(Channel::READ_EVENT);
        }
        if (events & EPOLLOUT) {
            ch->SetReadyEvents(Channel::WRITE_EVENT);
        }
        if (events & EPOLLET) {
            ch->SetReadyEvents(Channel::ET);
        }
        active_channels.push_back(ch);
    }
    return active_channels;
}

void Epoll::UpdateChannel(Channel *ch) {
    int sockfd = ch->GetSocket()->GetFd();
    struct epoll_event ev {};
    ev.data.ptr = ch;
    if (ch->GetListenEvents() & Channel::READ_EVENT) {
        ev.events |= EPOLLIN | EPOLLPRI;
    }
    if (ch->GetListenEvents() & Channel::WRITE_EVENT) {
        ev.events |= EPOLLOUT;
    }
    if (ch->GetListenEvents() & Channel::ET) {
        ev.events |= EPOLLET;
    }
    if (!ch->GetExist()) {
        ErrorIf(epoll_ctl(fd_, EPOLL_CTL_ADD, sockfd, &ev) == -1, "epoll add error");
        ch->SetExist();
    } else {
        ErrorIf(epoll_ctl(fd_, EPOLL_CTL_MOD, sockfd, &ev) == -1, "epoll modify error");
    }
}

void Epoll::DeleteChannel(Channel *ch) {
    int sockfd = ch->GetSocket()->GetFd();
    ErrorIf(epoll_ctl(fd_, EPOLL_CTL_DEL, sockfd, nullptr) == -1, "epoll delete error");
    ch->SetExist(false);
}