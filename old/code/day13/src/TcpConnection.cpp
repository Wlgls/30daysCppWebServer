#include "TcpConnection.h"

#include <unistd.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <utility>

#include "Buffer.h"
#include "Channel.h"
#include "Socket.h"
#include "util.h"


TcpConnection::TcpConnection(EventLoop *loop, Socket *sock) : loop_(loop), sock_(sock){
    if(loop_!=nullptr){
        channel_ = new Channel(loop_, sock_);
        channel_->EnableRead();
        channel_->UseET();
    }
    read_buffer_ = new Buffer();
    send_buffer_ = new Buffer();
    state_ = State::Connected;
}

TcpConnection::~TcpConnection(){
    if(loop_ != nullptr){
        delete channel_;
    }
    delete sock_;
    delete read_buffer_;
    delete send_buffer_;
}

void TcpConnection::Read() {

    ErrorIf(state_ != State::Connected, "connection state is disconnected!");
    read_buffer_->Clear();
    ReadNonBlocking();

}
void TcpConnection::Write() {
    ErrorIf(state_ != State::Connected, "connection state is disconnected!");
    WriteNonBlocking();
    send_buffer_->Clear();
}


void TcpConnection::ReadNonBlocking(){
    int sockfd = sock_->GetFd();
    char buf[1024];
    while(true){
        memset(buf, 0, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0 ){
            read_buffer_->Append(buf, bytes_read);
        }else if(bytes_read == -1 && errno == EINTR){
            printf("continue reading\n");
            continue;
        }else if(bytes_read == -1 && ((errno==EAGAIN) || (errno==EWOULDBLOCK))){
            break;
        }else if(bytes_read == 0){ // EOF 客户端断开连接
            printf("read EOF, client fd %d disconnected", sockfd);
            state_ = State::Closed;
            Close();
            break;
        }else{
            printf("Other error on client fd %d\n", sockfd);
            state_ = State::Closed;
            Close();
            break;
        }
    }
}

void TcpConnection::WriteNonBlocking(){
    int sockfd = sock_->GetFd();
    char buf[send_buffer_->Size()];
    memcpy(buf, send_buffer_->ToStr(), send_buffer_->Size());
    int data_size = send_buffer_->Size();
    int data_left = data_size;
    while(data_left > 0){
        ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left);
        if (bytes_write == -1 && errno == EINTR) {
            printf("continue writing\n");
            continue;
        }
        if (bytes_write == -1 && errno == EAGAIN) {
            break;
        }
        if (bytes_write == -1) {
            printf("Other error on client fd %d\n", sockfd);
            state_ = State::Closed;
            break;
        }
        data_left -= bytes_write;
    }
}




void TcpConnection::Send(std::string msg){
    SetSendBuffer(msg.c_str());
    Write();
}

void TcpConnection::Business(){
    Read();
    on_message_callback_(this);
}

void TcpConnection::Close() { delete_connection_callback_(sock_); }

TcpConnection::State TcpConnection::GetState() { return state_; }
void TcpConnection::SetSendBuffer(const char *str) { send_buffer_->SetBuf(str); }
Buffer *TcpConnection::GetReadBuffer() { return read_buffer_; }
const char *TcpConnection::ReadBuffer() { return read_buffer_->ToStr(); }
Buffer *TcpConnection::GetSendBuffer() { return send_buffer_; }
const char *TcpConnection::SendBuffer() { return send_buffer_->ToStr(); }

void TcpConnection::SetDeleteConnectionCallback(std::function<void(Socket *)> const &callback) {
    delete_connection_callback_ = callback;
}
void TcpConnection::SetOnConnectCallback(std::function<void(TcpConnection *)> const &callback) {
    on_connect_callback_ = callback;
}

void TcpConnection::SetOnMessageCallback(std::function<void(TcpConnection *)> const &callback) {
    on_message_callback_ = callback;
    std::function<void()> bus = std::bind(&TcpConnection::Business, this);
    channel_->SetReadCallback(bus);
}

void TcpConnection::GetlineSendBuffer() { send_buffer_->Getline(); }

Socket *TcpConnection::GetSocket() { return sock_; }
