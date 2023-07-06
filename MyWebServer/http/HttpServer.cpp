#include "HttpServer.h"
#include "HttpResponse.h"
#include "HttpRequest.h"
#include "HttpContext.h"
#include "Acceptor.h"
#include "TcpServer.h"
#include "TcpConnection.h"
#include "Buffer.h"
#include "EventLoop.h"

#include <functional>
#include <iostream>

 
void HttpServer::HttpDefaultCallBack(const HttpRequest& request, HttpResponse *resp){
    resp->SetStatusCode(HttpResponse::HttpStatusCode::k404NotFound);
    resp->SetStatusMessage("Not Found");
    resp->SetCloseConnection(true);
}

HttpServer::HttpServer(EventLoop * loop, const char *ip, const int port) : loop_(loop) {
    server_ = std::make_unique<TcpServer>(loop_, ip, port);
    server_->set_connection_callback(
        std::bind(&HttpServer::HandleConnection, this, std::placeholders::_1));

    server_->set_message_callback(
        std::bind(&HttpServer::HandleMessage, this, std::placeholders::_1)
    );
    SetHttpCallback(std::bind(&HttpServer::HttpDefaultCallBack, this, std::placeholders::_1, std::placeholders::_2));

    std::cout << "HttpServer Listening on " << ip << ":" << port << std::endl;
};

HttpServer::~HttpServer(){
};

void HttpServer::HandleConnection(const TcpConnectionPtr &conn){
}

void HttpServer::HandleMessage(const TcpConnectionPtr &conn){
    
    if (conn->state() == TcpConnection::ConnectionState::Connected)
    {
        HttpContext *context = conn->context();
        // 解析失败
        if (!context->ParaseRequest(conn->read_buf()->c_str(), conn->read_buf()->Size()))
        {
            conn->Send("HTTP/1.1 400 Bad Request\r\n\r\n");
            //conn->HandleClose();
            //conn->Shutdown();
        }
        // 解析成功
        if (context->GetCompleteRequest())
        {
            HandleRequest(conn, *context->request());
            context->ResetContextStatus();
        }
    }
}

void HttpServer::SetHttpCallback(const HttpServer::HttpResponseCallback &cb){
    response_callback_ = std::move(cb);
}

void HttpServer::HandleRequest(const TcpConnectionPtr &conn, const HttpRequest &request){
    std::string connection_state = request.GetHeader("Connection");
    bool close = (connection_state == "Close" ||
                  request.version() == HttpRequest::Version::kHttp10 &&
                  connection_state != "keep-alive");
    HttpResponse response(close);
    response_callback_(request, &response);

    conn->Send(response.message().c_str());
    if(response.IsCloseConnection()){
        //conn->HandleClose();
        // conn->Shutdown();
    }
}

void HttpServer::start(){
    server_->Start();
}

