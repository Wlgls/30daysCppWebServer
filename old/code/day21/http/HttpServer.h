

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

    void onConnection(const TcpConnectionPtr &conn);
    void onMessage(const TcpConnectionPtr &conn);
    void onRequest(const TcpConnectionPtr &conn, const HttpRequest &request);

    void TestTimer_IntervalEvery3Seconds() const {
        printf("%s TestTimer_IntervalEvery3Seconds\n", TimeStamp::Now().ToFormattedString().data());
    }

    void TestTimer_IntervalEvery5Seconds() const {
        printf("%s TestTimer_IntervalEvery5Seconds\n", TimeStamp::Now().ToFormattedString().data());
    }

    void TestTimer_IntervalEvery10Seconds() const {
        printf("%s TestTimer_IntervalEvery10Seconds\n", TimeStamp::Now().ToFormattedString().data());
    }

    void TestTimer_Interval15SecondsAfterOneLoop() const {
        printf("%s TestTimer_Interval15SecondsAfterOneLoop\n", TimeStamp::Now().ToFormattedString().data());
    }

private:
    EventLoop *loop_;
    std::unique_ptr<TcpServer> server_;

    HttpResponseCallback response_callback_;
};
