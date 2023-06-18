

/*
对http server的处理
*/


#pragma once
#include <functional>
#include <memory>
#include <stdio.h>
#include "common.h"
#include "timestamp.h"
#include "timer.h"
#include "timerqueue.h"

#define AUTOCLOSETIMEOUT 10
class TcpServer;
class TcpConnection;
class HttpRequest;
class HttpResponse;
class EventLoop;
class HttpServer
{
public:

    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

    typedef std::function<void(const HttpRequest &,
                               HttpResponse *)> HttpResponseCallback;
    DISALLOW_COPY_AND_MOVE(HttpServer);
    HttpServer(EventLoop *loop, const char *ip, const int port, bool auto_close_conn);
    ~HttpServer();

    void HttpDefaultCallBack(const HttpRequest &request, HttpResponse *resp);

    void SetHttpCallback(const HttpResponseCallback &cb);

    void CloseConn(std::weak_ptr<TcpConnection> &conn);

    void start();

    void onConnection(const TcpConnectionPtr &conn);
    void onMessage(const TcpConnectionPtr &conn);
    void onRequest(const TcpConnectionPtr &conn, const HttpRequest &request);


private:
    EventLoop *loop_;
    std::unique_ptr<TcpServer> server_;

    bool auto_close_conn_;

    HttpResponseCallback response_callback_;
};
