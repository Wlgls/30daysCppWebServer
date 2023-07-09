#include "TcpConnection.h"

#include <memory>
#include <unistd.h>
#include "Buffer.h"
#include "Channel.h"
#include "Socket.h"
#include "common.h"
#include <assert.h>
#include "../HTTP/HttpContext.h"


TcpConnection::TcpConnection(EventLoop *loop, int connfd){
    socket_ = std::make_unique<Socket>();
    socket_->SetFd(connfd);
    if(loop != nullptr){
        channel_ = std::make_unique<Channel>(connfd, loop);
        //channel_ = new Channel(connfd, loop);
        channel_->EnableRead();
        channel_->EnableET();
        //channel_->Tie(shared_from_this());
    }
    read_buf_ = std::make_unique<Buffer>();
    send_buf_ = std::make_unique<Buffer>();

    context_ = std::make_unique<HttpContext>();
    // state_ = ConnectionState::Connected;
}

TcpConnection::~TcpConnection(){};


void TcpConnection::ConnectionEstablished(){
    state_ = ConnectionState::Connected;
    //channel_->EnableRead();
    //channel_->EnableET();
    channel_->Tie(shared_from_this());
    if (on_connect_){
        on_connect_(shared_from_this());
    }
}



void TcpConnection::set_connection_callback(std::function<void(const std::shared_ptr<TcpConnection> &)> const &fn){
    on_connect_ = std::move(fn);
}

void TcpConnection::set_close_callback(std::function<void(int)> const &fn) { on_close_ = std::move(fn); }
void TcpConnection::set_message_callback(std::function<void(const std::shared_ptr<TcpConnection> &)> const &fn) { 
    on_message_ = std::move(fn);
    std::function<void()> cb = std::bind(&TcpConnection::OnMessage, this);
    channel_->set_read_callback(cb);
}


void TcpConnection::OnClose() {
    if (state_ != ConnectionState::Disconected){
        state_ = ConnectionState::Disconected;
        if(on_close_){
            on_close_(socket_->fd());
        }
    }
}

void TcpConnection::OnMessage(){
    Read();
    if (on_message_)
    {
        on_message_(shared_from_this());
    }
}

Socket *TcpConnection::socket() const { return socket_.get(); }

TcpConnection::ConnectionState TcpConnection::state() const { return state_; }

void TcpConnection::set_send_buf(const char *str) { send_buf_->set_buf(str); }
Buffer *TcpConnection::read_buf(){ return read_buf_.get(); }
Buffer *TcpConnection::send_buf() { return send_buf_.get(); }

RC TcpConnection::Send(std::string &msg){
    set_send_buf(msg.c_str());
    Write();
    return RC_SUCCESS;
}

RC TcpConnection::Send(const char *msg){
    set_send_buf(msg);
    Write();
    return RC_SUCCESS;
}


RC TcpConnection::Read()
{
    if(state_ != ConnectionState::Connected){
        perror("connection is not connected, cant read");
        return RC_CONNECTION_ERROR;
    }
    assert(state_ == ConnectionState::Connected && "connection state is disconnected!");
    read_buf_->Clear();
    RC rc = RC_UNDEFINED;
    rc = ReadNonBlocking();
    return rc;
}

RC TcpConnection::Write(){
    if (state_ != ConnectionState::Connected)
    {
        perror("connection is not connected, cant write  12314");
        return RC_CONNECTION_ERROR;
    }

    RC rc = RC_UNDEFINED;
    rc = WriteNonBlocking();
    send_buf_->Clear();

    return rc;
}


RC TcpConnection::ReadNonBlocking(){
    int sockfd = socket_->fd();
    char buf[1024];
    while(true){
        memset(buf, 0, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0){
            read_buf_->Append(buf, bytes_read);
        }else if(bytes_read == -1 && errno == EINTR){
            printf("continue reading\n");
            continue;
        }else if((bytes_read == -1) && (
            (errno == EAGAIN) || (errno == EWOULDBLOCK))){
            break;
        }else if (bytes_read == 0){//
            OnClose();
            break;
        }else{
            OnClose();
            break;
        }
    }
    return RC_SUCCESS;
}

RC TcpConnection::WriteNonBlocking(){
    int sockfd = socket_->fd();
    char buf[send_buf_->Size()];
    memcpy(buf, send_buf_->c_str(), send_buf_->Size());
    int data_size = send_buf_->Size();
    int data_left = data_size;

    while(data_left > 0){
        ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left);
        if(bytes_write == -1 && errno == EINTR){
            printf("continue writing\n");
            continue;
        }
        if(bytes_write == -1 && errno == EAGAIN){
            break;
        }
        if(bytes_write == -1){
            OnClose();
            break;
        }
        data_left -= bytes_write;
    }
    return RC_SUCCESS;
}

HttpContext * TcpConnection::context() const{
    return context_.get();
}