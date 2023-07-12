#include "Acceptor.h"
#include "EventLoop.h"
#include "TcpConnection.h"
#include "TcpServer.h"
#include "Buffer.h"
#include <thread>
#include <iostream>
#include <functional>
#include <arpa/inet.h>
#include <vector>
#include <stdio.h>
#include <memory>
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <thread>
#include <memory>
#include <unistd.h>
#include <assert.h>
#include <iostream>
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

std::string read_image(const std::string& image_path){
    std::ifstream is(image_path.c_str(), std::ifstream::in);
    is.seekg(0, is.end);
    int flength = is.tellg();
    is.seekg(0, is.beg);
    char * buffer = new char[flength];
    is.read(buffer, flength);
    std::string image(buffer, flength);
    return image;
}


void EchoServer::onConnection(const std::shared_ptr<TcpConnection> & conn){
    // 获取接收连接的Ip地址和port端口
    int clnt_fd = conn->fd();
    struct sockaddr_in peeraddr;
    socklen_t peer_addrlength = sizeof(peeraddr);
    getpeername(clnt_fd, (struct sockaddr *)&peeraddr, &peer_addrlength);

    std::cout << std::this_thread::get_id()
              << " EchoServer::OnNewConnection : new connection "
              << "[fd#" << clnt_fd << "]"
              << " from " << inet_ntoa(peeraddr.sin_addr) << ":" << ntohs(peeraddr.sin_port)
              << std::endl;
};

void EchoServer::onMessage(const std::shared_ptr<TcpConnection> & conn){
    // std::cout << std::this_thread::get_id() << " EchoServer::onMessage" << std::endl;
    if (conn->state() == TcpConnection::ConnectionState::Connected)
    {
        std::string a = "HTTP/1.1 200 \r\n"
                        "Content-Length: 10526 \r\n"
                        "Connection: Keep-Alive \r\n"
                        "Content-Type: image/jpeg \r\n"
                        "\r\n";
        a += read_image("../static/cat.jpg");
        int bodyl = a.length();
        const char *body = a.data();

        char *buffer = new char[bodyl];
        memcpy(buffer, body, bodyl);

        ::write(conn->fd(), buffer, bodyl);
        // std::cout << conn->Send(body.data(), body.length());
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