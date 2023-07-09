
#pragma once
#include <string>
#include <cstring>

class InetAddress {
    private:
        const char *ip_;
        const int port_;
    
    public:
        InetAddress(const char *ip, const int port);
        ~InetAddress();

        const char *ip() const;
        int port() const;

        const std::string IpPortToString() const;
};