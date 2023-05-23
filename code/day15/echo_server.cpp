#include "TCP/Acceptor.h"
#include "TCP/EventLoop.h"
#include "TCP/TcpServer.h"
#include "TCP/Buffer.h"
#include "TCP/ThreadPool.h"
#include "TCP/Socket.h"
#include "TCP/TcpConnection.h"
#include <iostream>
#include <functional>

class EchoServer{
    public:
        EchoServer();
        ~EchoServer();

        void start();
        void onConnection(const std::shared_ptr<TcpConnection> & conn);
        void onMessage(const std::shared_ptr<TcpConnection> & conn);

    private:
        TcpServer server_;
};

EchoServer::EchoServer() :  server_("127.0.0.1", 1233){
    server_.set_connection_callback(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
    server_.set_message_callback(std::bind(&EchoServer::onMessage, this, std::placeholders::_1));
};
EchoServer::~EchoServer(){};

void EchoServer::start(){
    server_.Start();
}

void EchoServer::onConnection(const std::shared_ptr<TcpConnection> & conn){
    std::cout << "New connection fd:" << conn->socket()->fd() << std::endl;
};

void EchoServer::onMessage(const std::shared_ptr<TcpConnection> & conn){
    std::cout << "Message from clent " << conn->read_buf()->c_str() << std::endl;
    if(conn->state() == TcpConnection::ConnectionState::Connected){
        conn->Send(conn->read_buf()->c_str());
    }
}

int main(int argc, char *argv[]) {
    EchoServer *server = new EchoServer();

    server->start();

    delete server;
    return 0;
}