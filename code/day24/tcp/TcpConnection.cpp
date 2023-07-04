#include "TcpConnection.h"

#include <memory>
#include <unistd.h>
#include "Buffer.h"
#include "Channel.h"
#include "common.h"
#include <assert.h>
#include "EventLoop.h"
#include "HttpContext.h"


TcpConnection::TcpConnection(EventLoop *loop, int connfd, int connid): connfd_(connfd), connid_(connid), loop_(loop){

    if (loop != nullptr)
    {
        channel_ = std::make_unique<Channel>(connfd, loop);
        channel_->EnableRead();
        channel_->EnableET();
        channel_->set_read_callback(std::bind(&TcpConnection::OnMessage, this));
    }
    read_buf_ = std::make_unique<Buffer>();
    send_buf_ = std::make_unique<Buffer>();
    context_ = std::make_unique<HttpContext>();
}

TcpConnection::~TcpConnection(){
};

void TcpConnection::ConnectionEstablished(){
    state_ = ConnectionState::Connected;
    channel_->Tie(shared_from_this());
    if (on_connect_){
        on_connect_(shared_from_this());
    }
}

void TcpConnection::ConnectionDestructor(){
    loop_->DeleteChannel(channel_.get());
    ::close(connfd_);
}

void TcpConnection::set_connection_callback(std::function<void(const std::shared_ptr<TcpConnection> &)> const &fn){
    on_connect_ = std::move(fn);
}

void TcpConnection::set_close_callback(std::function<void(const std::shared_ptr<TcpConnection> &)> const &fn) { on_close_ = std::move(fn); }
void TcpConnection::set_message_callback(std::function<void(const std::shared_ptr<TcpConnection> &)> const &fn) { 
    on_message_ = std::move(fn);
    
}


void TcpConnection::OnClose() {
    if (state_ != ConnectionState::Disconected){
        state_ = ConnectionState::Disconected;
        if(on_close_){
            on_close_(shared_from_this());
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

EventLoop *TcpConnection::loop() const { return loop_; }
int TcpConnection::fd() const { return connfd_; }
int TcpConnection::id() const { return connid_; }

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
        perror("connection is not connected, cant write");
        return RC_CONNECTION_ERROR;
    }

    RC rc = RC_UNDEFINED;
    rc = WriteNonBlocking();
    send_buf_->Clear();

    return rc;
}


RC TcpConnection::ReadNonBlocking(){
    char buf[1024];
    while(true){
        memset(buf, 0, sizeof(buf));
        ssize_t bytes_read = read(connfd_, buf, sizeof(buf));
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
    char buf[send_buf_->Size()];
    memcpy(buf, send_buf_->c_str(), send_buf_->Size());
    int data_size = send_buf_->Size();
    int data_left = data_size;

    while(data_left > 0){
        ssize_t bytes_write = write(connfd_, buf + data_size - data_left, data_left);
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

TimeStamp TcpConnection::timestamp() const { return timestamp_; }
void TcpConnection::UpdateTimeStamp(TimeStamp now){
    timestamp_ = now;
}