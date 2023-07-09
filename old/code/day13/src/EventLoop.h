/*

作为Reactor模式的服务器。

将整个服务器抽象成一个Server类，这个类中有一个main_reactor，

里面的核心是EventLoop。是一个事件循环。其主要作用就是监听epoll中上的事件，并对不同事件类型进行不同的处理。

*/

#pragma once
#include <functional>

class Epoll;
class Channel;
class EventLoop{
    public:
        EventLoop();
        ~EventLoop();

        void Loop();
        void UpdateChannel(Channel *ch);
        void DeleteChannel(Channel *ch);
        void Quit();

    private:
        Epoll *epoll_{nullptr};
        bool quit_{false};
};