/*
Channel类,简单的socket类无法对文件描述符进行深度的处理,因此使用Channel类用于存储相应的信息.

在本次实现中，定义了几种对socket状态的更改。如使用ET,监听读等。

当有事件发生时，将通过传入的回调函数进行对事件的处理。这里定义了read call_back和write call_back.尽管在本项目中只使用了readcallback。

在每一个channel创建时，都需要确定回调函数     

*/

#pragma once

#include <functional>

class Socket;
class EventLoop;
class Channel {
    public:
        Channel(EventLoop *loop, Socket *socket);
        ~Channel();

        void HandleEvent();
        void EnableRead();
        void EnableWrite();

        Socket *GetSocket();
        int GetListenEvents();
        int GetReadyEvents();
        bool GetExist();
        void SetExist(bool in = true);
        void UseET();

        void SetReadyEvents(int ev);
        void SetReadCallback(std::function<void()> const &callback);
        void SetWriteCallback(std::function<void()> const &callback);

        static const int READ_EVENT;   // NOLINT
        static const int WRITE_EVENT;  // NOLINT
        static const int ET;           // NOLINT

    private:
        EventLoop *loop_;
        Socket *socket_;
        int listen_events_{0};
        int ready_events_{0};
        bool exist_{false};
        std::function<void()> read_callback_;
        std::function<void()> write_callback_;
};