#include "InetAddress.h"

#include <string>
#include <cstring>

InetAddress::InetAddress(const char *ip, const int port) : ip_(ip), port_(port){};
InetAddress::~InetAddress(){};
const char *InetAddress::ip() const { return ip_; }
int InetAddress::port() const { return port_; }
const std::string InetAddress::IpPortToString() const{
    char buf[32];
    int len = snprintf(buf, sizeof(buf), "%s:%d", ip_, port_);
    return std::string(buf, buf + len);
}