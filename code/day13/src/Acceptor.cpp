#include "Acceptor.h"
#include <utility>
#include <stdio.h>
#include "Channel.h"
#include "Socket.h"

Acceptor::Acceptor(EventLoop *loop): loop_(loop), sock_(nullptr), acceptChannel_(nullptr){
    sock_ = new Socket();
    InetAddress *addr = new InetAddress("127.0.0.1", 1234);
    sock_->Bind(addr);

    sock_->Listen();

    acceptChannel_ = new Channel(loop_, sock_);

    std::function<void()> cb = std::bind(&Acceptor::AcceptConnection, this);
    acceptChannel_->SetReadCallback(cb); // 对于服务器而言，回调函数是一个接受连接的函数。
    acceptChannel_->EnableRead();
    delete addr;
}

Acceptor::~Acceptor(){
    delete acceptChannel_;
    delete sock_;
}

void Acceptor::AcceptConnection(){ 
    InetAddress *clnt_addr = new InetAddress();
    Socket *clnt_sock = new Socket(sock_->Accept(clnt_addr));
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->GetFd(), clnt_addr->GetIp(), clnt_addr->GetPort());

    
    clnt_sock->SetNonBlocking();
    if (new_connection_callback_){
        new_connection_callback_(clnt_sock);
    }
    delete clnt_addr;
}

void Acceptor::SetNewConnectionCallback(std::function<void(Socket *)> const &callback) {
    new_connection_callback_ = callback;
}