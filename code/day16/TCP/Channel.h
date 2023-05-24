/*
Channel类,简单的socket类无法对文件描述符进行深度的处理,因此使用Channel类用于存储相应的信息.

在本次实现中，定义了几种对socket状态的更改。如使用ET,监听读等。

当有事件发生时，将通过传入的回调函数进行对事件的处理。
这里定义了read call_back和write call_back.

在每一个channel创建时，都需要确定回调函数     
*/

#pragma once

#include <functional>
#include "common.h"
#include <memory>

class EventLoop;
class Channel {
    public:
        DISALLOW_COPY_AND_MOVE(Channel);
        Channel(int fd, EventLoop * loop);
        
        ~Channel();

        void HandleEvent() const; // 处理事件
        void HandleEventWithGuard() const;
        void EnableRead();  // 允许读
        void EnableWrite(); // 允许写
        void EnableET(); // 以ET形式触发


        int fd() const;  // 获取fd
        short listen_events() const; // 监听的事件
        short ready_events() const; // 准备好的事件

        bool IsInEpoll() const; // 判断当前channel是否在poller中
        void SetInEpoll(bool in = true); // 设置当前状态为poller中
        

        void SetReadyEvents(short ev);
        void set_read_callback(std::function<void()> const &callback);// 设置回调函数
        void set_write_callback(std::function<void()> const &callback);

        void Tie(const std::shared_ptr<void> &ptr); // 设定tie
        static const short READ_EVENT;    // 事件定义
        static const short WRITE_EVENT;  
        static const short ET;           
    private:
        int fd_;
        EventLoop *loop_;
        
        short listen_events_;
        short ready_events_;
        bool in_epoll_{false};
        std::function<void()> read_callback_;
        std::function<void()> write_callback_;

        bool tied_;
        std::weak_ptr<void> tie_;

};