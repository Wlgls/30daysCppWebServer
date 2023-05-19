/*
最底层的库,对socket和address的封装
*/

#pragma once
#include <arpa/inet.h>

class InetAddress{
    public:
        InetAddress();
        InetAddress(const char *ip, uint16_t port);
        ~InetAddress() = default;

        void SetAddr(sockaddr_in addr);
        sockaddr_in GetAddr();
        const char *GetIp();
        uint16_t GetPort();
    private:
        struct sockaddr_in addr_ {
        };
};

class Socket{
    private:
        int fd_{-1};
    public:
        Socket();
        explicit Socket(int fd);
        ~Socket();
        void Bind(InetAddress *addr); // 将socket与地址绑定
        void Listen(); // 监听
        int Accept(InetAddress *addr); // 接受一个客户端连接

        void Connect(InetAddress *addr);
        void Connect(const char *ip, uint16_t port); // 尝试连接

        void SetNonBlocking();
        bool IsNonBlocking();

        int GetFd();
};
