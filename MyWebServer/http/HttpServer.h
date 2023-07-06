
#pragma once
#include "common.h"
#include <functional>
#include <memory>
#include <stdio.h>


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
    HttpServer(EventLoop *loop, const char *ip, const int port);
    ~HttpServer();

    void HttpDefaultCallBack(const HttpRequest &request, HttpResponse *resp);

    void SetHttpCallback(const HttpResponseCallback &cb);

    void start();

    void HandleConnection(const TcpConnectionPtr &conn);
    void HandleMessage(const TcpConnectionPtr &conn);
    void HandleRequest(const TcpConnectionPtr &conn, const HttpRequest &request);


private:
    EventLoop *loop_;
    std::unique_ptr<TcpServer> server_;

    bool auto_close_conn_;

    HttpResponseCallback response_callback_;
};
