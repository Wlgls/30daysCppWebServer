/*
使用epoll监听事件，

在这个版本中，将Channel加入到Epoll红黑树中， 并获得当前epoll中有事件发生的channel，并将结果返回给channel。

除此之外，还需要加入或者更新或者删除一个channel。
*/



#pragma once
#include<vector>

#include<sys/epoll.h>

class Channel;
class Epoll{
    public:
        Epoll();
        ~Epoll();

        void UpdateChannel(Channel *ch); // 将Channel 加入到Epoll中，然后更新
        void DeleteChannel(Channel *ch);

        std::vector<Channel *> Poll(int timeout = -1); // 获取当前有事件发生的channel

    private:
        int fd_{-1};
        struct epoll_event *events_{nullptr};
};
