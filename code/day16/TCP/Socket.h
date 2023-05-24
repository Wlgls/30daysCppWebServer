/*
最底层的库,对socket的封装
*/

#pragma once
#include <string>
#include <cstring>
#include "common.h"


class Socket{
    public:
        DISALLOW_COPY_AND_MOVE(Socket);

        Socket();
        ~Socket();

        void SetFd(int fd);
        int fd() const;

        std::string get_addr() const;

        RC Create(); // 创建socket
        RC Bind(const char *ip, const int port) const; // 绑定ip地址
        RC Listen() const;             // 监听
        RC Accept(int &clnt_fd) const; // 接受客户端

        RC Connect(const char *ip, const int port) const; // 连接
        RC SetNonBlocking() const; // 设置非阻塞

        bool IsNonBlocking() const; // 判断是否是阻塞

    private:
        int fd_;
};
