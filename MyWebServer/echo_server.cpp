#include "TCP/Acceptor.h"
#include "TCP/EventLoop.h"
#include "TCP/TcpServer.h"
#include "TCP/Buffer.h"
#include "TCP/ThreadPool.h"
#include "TCP/Socket.h"
#include "TCP/TcpConnection.h"
#include <iostream>
#include <functional>


int main(int argc, char *argv[]) {
    
    TcpServer *server = new TcpServer("127.0.0.1", 1234);

    server->set_connection_callback([](TcpConnection *conn)
                                    { std::cout << "New connection fd: " << conn->socket()->fd() << std::endl; });

    server->set_message_callback([](TcpConnection *conn)
                                 {
        std::cout << "Message from client " << conn->read_buf()->c_str() << std::endl;
        conn->Send(conn->read_buf()->c_str()); });

    server->Start();

    delete server;
    return 0;
}