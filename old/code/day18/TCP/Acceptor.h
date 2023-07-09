
#pragma once


#include "common.h"
#include <memory>
#include <functional>

class Socket;
class Channel;
class Acceptor
{
public:
    DISALLOW_COPY_AND_MOVE(Acceptor);
    Acceptor(EventLoop *loop, const char * ip, const int port);
    ~Acceptor();

    RC AcceptConnection() const;
    void set_newconnection_callback(std::function<void(int)> const &callback);

    RC Bind(const char *ip, const int port) const;
    RC Create();
    RC Listen() const;
    RC Accept(int &clnt_fd) const;

private:
    //std::unique_ptr<Socket> socket_;
    EventLoop *loop_;
    int listenfd_;
    std::unique_ptr<Channel> channel_;
    std::function<void(int)> new_connection_callback_;
};