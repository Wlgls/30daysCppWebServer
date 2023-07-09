#include <iostream>
#include "src/TcpServer.h"
#include "src/Buffer.h"
#include "src/TcpConnection.h"
#include "src/EventLoop.h"
#include "src/Socket.h"


class EchoServer{
    public:
        EchoServer(EventLoop *loop);
        ~EchoServer();

        void start();
        void onConnection(TcpConnection *conn);
        void onMessage(TcpConnection *conn);

    private:
        EventLoop *loop_;
        TcpServer server_;
};

EchoServer::EchoServer(EventLoop *loop) : loop_(loop), server_(loop){
    server_.OnConnect(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
    server_.OnMessage(std::bind(&EchoServer::onMessage, this, std::placeholders::_1));
};
EchoServer::~EchoServer(){};

void EchoServer::start(){
    loop_->Loop();
}

void EchoServer::onConnection(TcpConnection *conn){
    std::cout << "New connection fd:" << conn->GetSocket()->GetFd() << std::endl;
};

void EchoServer::onMessage(TcpConnection *conn){
    std::cout << "Message from clent " << conn->ReadBuffer() << std::endl;
    if(conn->GetState() == TcpConnection::State::Connected){
        conn->Send(conn->ReadBuffer());
    }
}

int main(){
    EventLoop *loop = new EventLoop();
    EchoServer *server = new EchoServer(loop);

    server->start();
    delete loop;
    delete server;
    return 0;
}