#include "tcp/Acceptor.h"
#include "tcp/EventLoop.h"
#include "tcp/TcpServer.h"
#include "tcp/Buffer.h"
#include "tcp/ThreadPool.h"
#include "tcp/TcpConnection.h"
#include <iostream>
#include <functional>


int main(int argc, char *argv[]) {
    
    TcpServer *server = new TcpServer("127.0.0.1", 1234);

    server->set_message_callback([](TcpConnection *conn)
                                 {
        std::cout << "Message from client " << conn->id() << " is " << conn->read_buf()->c_str() << std::endl;
        conn->Send(conn->read_buf()->c_str()); });

    server->Start();

    delete server;
    return 0;
}