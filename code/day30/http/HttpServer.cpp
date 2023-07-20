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
#include <fstream>
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
        LOG_INFO << "\n"
                 << conn->read_buf()->PeekAllAsString();
        
        
        if(auto_close_conn_)
             // 保存最近一次活跃的时间
            conn->UpdateTimeStamp(TimeStamp::Now());

        HttpContext *context = conn->context();
        if (!context->ParaseRequest(conn->read_buf()->PeekAllAsString()) && !conn->cur_is_file()) // 并且也不是文件
        {
            
            LOG_INFO << "HttpServer::onMessage : Receive non HTTP message";
            conn->Send("HTTP/1.1 400 Bad Request\r\n\r\n");
            conn->HandleClose();
        }
        
        if(conn->cur_is_file()){
            // 这意味着当前接收到是一个文件
            // 首先比对boundary是否正确
            // 这里应该可以优化，直接在buffer中实现对`\r\n`的寻找就不需要频繁的赋值了
            std::string message = conn->read_buf()->PeekAllAsString();
            size_t found = message.find("\r\n"); // 找到第一个`\r\n`,表示寻找boundary值
            if (found != std::string::npos){
                if (conn->boundary() == message.substr(0, found))
                {
                    // 说明边界对应了，正确
                    message.erase(0, found + 2);// 去掉第一行，方便后续读取。如果直接在buffer中实现了对\r\n的寻找，可以通过修改可读索引来控制
                }
                else
                {
                    LOG_ERROR << "boundary not equal";
                    return;
                }
            }

            // 接下来将进一步处理文件部分，主要由两个部分组成，Header和Body。对于Header,我们需要提取出`filename`作为文件名。
            std::string strline;
            std::string filename;
            while (1)
            {
                found = message.find("\r\n");
                if (found != std::string::npos){
                    strline = message.substr(0, found + 2);// 包含\r\n
                    message.erase(0, found + 2);
                    if(strline == "\r\n"){
                        // 说明接下来是文件内容
                        UpLoadFile(filename, message, conn->boundary());
                        break;
                    }
                    // 寻找文件名
                    found = strline.find("filename");
                    if(found != std::string::npos){ 
                        // 包含`"\r\n`
                        LOG_INFO << strline << "   " << strline.size() - 4;
                        strline.erase(0, found + std::string("filename=\"").size());// 是放掉这些数据，可以方便更好的定位filename的位置。也可以直接换算
                        filename = strline.substr(0, strline.size() - 3);
                        LOG_INFO << "FILENAME::" << filename << "1234";
                    }
                }else{
                    LOG_ERROR << "recv ERROR";
                    break;
                }
            }
            conn->set_cur_is_file(false);
            OnFile(conn);
            return;
        }
        
        if (context->GetCompleteRequest())
        {
            onRequest(conn, *context->request());
        }
        conn->read_buf()->RetrieveAll();
        context->ResetContextStatus();
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
    
    // 如果是文件的话
    if(request.GetHeader("Content-Type").substr(0, 19) == "multipart/form-data"){
        // 说明是文件, 直接返回
        std::string content_type = request.GetHeader("Content-Type");
        size_t found = content_type.find("boundary");
        if (found != std::string::npos)
        {
            conn->set_boundary("--" + content_type.substr(found + 9));
        }
        // LOG_INFO << conn->boundary();
        conn->set_cur_is_file(true);
        return;
    }

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
    }

    if(response.IsCloseConnection()){
        conn->HandleClose();
    }
}

void HttpServer::OnFile(const TcpConnectionPtr &conn){
    HttpResponse response(true);
    response.SetStatusCode(HttpResponse::HttpStatusCode::k302K);
    response.SetStatusMessage("Moved Temporarily");
    response.SetContentType("text/html");
    response.AddHeader("Location", "/fileserver");

    conn->Send(response.message());
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

void HttpServer::UpLoadFile(const std::string &filename, const std::string &message, const std::string &boundary){
    LOG_INFO << "UpLoadFile";
    size_t found = message.find(boundary);
    std::string filedata;
    if (found != std::string::npos)
    {
        filedata = message.substr(0, found);
    }

    std::ofstream ofs("../files/" + filename, std::ios::out | std::ios::app | std::ios::binary);
    ofs.write(filedata.data(), filedata.size());
    ofs.close();
}