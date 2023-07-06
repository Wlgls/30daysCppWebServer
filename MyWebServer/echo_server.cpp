#include "Acceptor.h"
#include "EventLoop.h"
#include "TcpServer.h"
#include "Buffer.h"
#include "ThreadPool.h"
#include "TcpConnection.h"
#include <iostream>
#include <functional>

class EchoServer{
    public:
        EchoServer(EventLoop *loop, const char *ip, const int port);
        ~EchoServer();

        void start();
        void onConnection(const std::shared_ptr<TcpConnection> & conn);
        void onMessage(const std::shared_ptr<TcpConnection> & conn);

    private:
        TcpServer server_;
};

EchoServer::EchoServer(EventLoop *loop, const char *ip, const int port) :  server_(loop, ip, port){
    server_.set_connection_callback(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
    server_.set_message_callback(std::bind(&EchoServer::onMessage, this, std::placeholders::_1));
};
EchoServer::~EchoServer(){};

void EchoServer::start(){
    server_.Start();
}

void EchoServer::onConnection(const std::shared_ptr<TcpConnection> & conn){
    //std::cout << "New connection fd:" << conn->fd() << std::endl;
};

void EchoServer::onMessage(const std::shared_ptr<TcpConnection> & conn){
    if(conn->state() == TcpConnection::ConnectionState::Connected){
        //std::cout << "Message from clent " << conn->read_buf()->c_str() << std::endl;
        //conn->Send(conn->read_buf()->c_str());
        conn->HandleClose();
    }
}

int main(int argc, char *argv[]){
    int port;
    if (argc <= 1)
    {
        port = 1234;
    }else if (argc == 2){
        port = atoi(argv[1]);
    }else{
        printf("error");
        exit(0);
    }
    EventLoop *loop = new EventLoop();
    EchoServer *server = new EchoServer(loop, "127.0.0.1", port);
    server->start();
    
    //delete loop;
    //delete server;
    return 0;
}