#include "Acceptor.h"
#include "EventLoop.h"
#include "TcpConnection.h"
#include "TcpServer.h"
#include "Buffer.h"
#include "CurrentThread.h"
#include <iostream>
#include <functional>
#include <arpa/inet.h>
#include <vector>

class EchoServer{
    public:
        EchoServer(EventLoop *loop, const char *ip, const int port);
        ~EchoServer();

        void start();
        void onConnection(const std::shared_ptr<TcpConnection> & conn);
        void onMessage(const std::shared_ptr<TcpConnection> & conn);

        void SetThreadNums(int thread_nums);

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
    // 获取接收连接的Ip地址和port端口
    int clnt_fd = conn->fd();
    struct sockaddr_in peeraddr;
    socklen_t peer_addrlength = sizeof(peeraddr);
    getpeername(clnt_fd, (struct sockaddr *)&peeraddr, &peer_addrlength);

    std::cout << CurrentThread::tid()
              << " EchoServer::OnNewConnection : new connection "
              << "[fd#" << clnt_fd << "]"
              << " from " << inet_ntoa(peeraddr.sin_addr) << ":" << ntohs(peeraddr.sin_port)
              << std::endl;
};

void EchoServer::onMessage(const std::shared_ptr<TcpConnection> & conn){
    // std::cout << CurrentThread::tid() << " EchoServer::onMessage" << std::endl;
    if (conn->state() == TcpConnection::ConnectionState::Connected)
    {
        std::cout << CurrentThread::tid() << "Message from clent " << conn->read_buf()->c_str() << std::endl;
        conn->Send(conn->read_buf()->c_str());
        conn->Shutdown();
    }
}

void EchoServer::SetThreadNums(int thread_nums) { server_.SetThreadNums(thread_nums); }

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
    int size = std::thread::hardware_concurrency();
    EventLoop *loop = new EventLoop();
    EchoServer *server = new EchoServer(loop, "127.0.0.1", port);
    server->SetThreadNums(size);
    server->start();

    // delete loop;
    // delete server;
    return 0;
}