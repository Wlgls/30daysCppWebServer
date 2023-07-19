#include "HttpServer.h"
#include "HttpResponse.h"
#include "HttpRequest.h"
#include "HttpContext.h"
#include "Acceptor.h"
#include "TcpServer.h"
#include "TcpConnection.h"
#include "Buffer.h"
#include "EventLoop.h"
#include "Logging.h"
#include "TimeStamp.h"
#include <arpa/inet.h>
#include <functional>
#include <iostream>

#include <unistd.h>

void HttpServer::HttpDefaultCallBack(const HttpRequest& request, HttpResponse *resp){
    resp->SetStatusCode(HttpResponse::HttpStatusCode::k404NotFound);
    resp->SetStatusMessage("Not Found");
    resp->SetCloseConnection(true);
}

HttpServer::HttpServer(EventLoop * loop, const char *ip, const int port, bool auto_close_conn) : loop_(loop), auto_close_conn_(auto_close_conn) {
    server_ = std::make_unique<TcpServer>(loop_, ip, port);
    server_->set_connection_callback(
        std::bind(&HttpServer::onConnection, this, std::placeholders::_1));

    server_->set_message_callback(
        std::bind(&HttpServer::onMessage, this, std::placeholders::_1)
    );
    SetHttpCallback(std::bind(&HttpServer::HttpDefaultCallBack, this, std::placeholders::_1, std::placeholders::_2));

    LOG_INFO << "HttpServer Listening on [ " << ip << ":" << port << " ]";

    //loop_->RunEvery(3.0, std::bind(&HttpServer::TestTimer_IntervalEvery3Seconds, this));
};

HttpServer::~HttpServer(){
};

void HttpServer::onConnection(const TcpConnectionPtr &conn){
    int clnt_fd = conn->fd();
    struct sockaddr_in peeraddr;
    socklen_t peer_addrlength = sizeof(peeraddr);
    getpeername(clnt_fd, (struct sockaddr *)&peeraddr, &peer_addrlength);

    LOG_INFO << "HttpServer::OnNewConnection : Add connection "
             << "[ fd#" << clnt_fd << "-id#" << conn->id() <<  " ]"
             << " from " << inet_ntoa(peeraddr.sin_addr) << ":" << ntohs(peeraddr.sin_port);

    if(auto_close_conn_){
        loop_->RunAfter(AUTOCLOSETIMEOUT, std::move(std::bind(&HttpServer::ActiveCloseConn, this, std::weak_ptr<TcpConnection>(conn))));
    }
}

void HttpServer::onMessage(const TcpConnectionPtr &conn){
    if (conn->state() == TcpConnection::ConnectionState::Connected)
    {
        //LOG_INFO << "\n"
        //         << conn->read_buf()->PeekAllAsString();
        
        
        if(auto_close_conn_)
             // 保存最近一次活跃的时间
            conn->UpdateTimeStamp(TimeStamp::Now());

        HttpContext *context = conn->context();
        if (!context->ParaseRequest(conn->read_buf()->RetrieveAllAsString()))
        {

            LOG_INFO << "HttpServer::onMessage : Receive non HTTP message";
            conn->Send("HTTP/1.1 400 Bad Request\r\n\r\n");
            conn->HandleClose();
        }

        if (context->GetCompleteRequest())
        {

            onRequest(conn, *context->request());
            context->ResetContextStatus();
        }
    }
}

void HttpServer::SetHttpCallback(const HttpServer::HttpResponseCallback &cb){
    response_callback_ = std::move(cb);
}

void HttpServer::onRequest(const TcpConnectionPtr &conn, const HttpRequest &request){
    LOG_INFO << "HttpServer::onMessage - Request URL : " << request.url() << " from TcpConnection"
             << "[ fd#" << conn->fd() << "-id#" << conn->id() << " ]";
    std::string connection_state = request.GetHeader("Connection");
    bool isclose = (connection_state == "Close" ||
                  (request.version() == HttpRequest::Version::kHttp10 &&
                  connection_state != "keep-alive"));
    HttpResponse response(isclose);
    response_callback_(request, &response);

    // 如果是HTML，直接发送所有信息
    if(response.bodytype() == HttpResponse::HttpBodyType::HTML_TYPE){
        conn->Send(response.message());
    }else{
        // 考虑到头部字段数据量不多，直接发送完头部字段后，直接发送文件。
        conn->Send(response.beforebody());
        //sleep(1);
        conn->SendFile(response.filefd(), response.GetContentLength());
        int ret = ::close(response.filefd());
        if(ret == -1){
            LOG_ERROR << "Close File Error";
        }else{
            LOG_INFO << "Close File Ok";
        }
        void(ret);
    }

    if(response.IsCloseConnection()){
        conn->HandleClose();
    }
}

void HttpServer::start(){
    server_->Start();
}

void HttpServer::SetThreadNums(int thread_nums) { server_->SetThreadNums(thread_nums); }

void HttpServer::ActiveCloseConn(std::weak_ptr<TcpConnection> & connection){
    TcpConnectionPtr conn = connection.lock(); //防止conn已经被释放
    if (conn)
    {
        if(TimeStamp::AddTime(conn->timestamp(), AUTOCLOSETIMEOUT) < TimeStamp::Now()){
            conn->HandleClose();
        }else{
            loop_->RunAfter(AUTOCLOSETIMEOUT, std::move(std::bind(&HttpServer::ActiveCloseConn, this, connection)));
        }
    }
}