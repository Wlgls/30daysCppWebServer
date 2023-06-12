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
        std::bind(&HttpServer::onConnection, this, std::placeholders::_1));

    server_->set_message_callback(
        std::bind(&HttpServer::onMessage, this, std::placeholders::_1)
    );
    SetHttpCallback(std::bind(&HttpServer::HttpDefaultCallBack, this, std::placeholders::_1, std::placeholders::_2));

    //loop_->RunEvery(3.0, std::bind(&HttpServer::TestTimer_IntervalEvery3Seconds, this));
    //loop_->RunEvery(5.0, std::bind(&HttpServer::TestTimer_IntervalEvery5Seconds, this));
};

HttpServer::~HttpServer(){
};

void HttpServer::onConnection(const TcpConnectionPtr &conn){
    std::cout << "New connection id: " << conn->id() << std::endl;
}

void HttpServer::onMessage(const TcpConnectionPtr &conn){
    if (conn->state() == TcpConnection::ConnectionState::Connected)
    {
        std::cout << "message from connection id: " << conn->id() << std::endl;
        HttpContext *context = conn->context();
        if (!context->ParaseRequest(conn->read_buf()->c_str(), conn->read_buf()->Size()))
        {
            conn->Send("HTTP/1.1 400 Bad Request\r\n\r\n");
            conn->OnClose();
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
    std::string connection_state = request.GetHeader("Connection");
    bool close = (connection_state == "Close" ||
                  request.version() == HttpRequest::Version::kHttp10 &&
                  connection_state != "keep-alive");
    HttpResponse response(close);
    response_callback_(request, &response);

    conn->Send(response.message().c_str());

    if(response.IsCloseConnection()){
        conn->OnClose();
    }
}

void HttpServer::start(){
    server_->Start();
}
