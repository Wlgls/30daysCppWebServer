

/*
对http server的处理
*/


#pragma once
#include <functional>
#include <memory>
class TcpServer;
class TcpConnection;
class HttpRequest;
class HttpResponse;

class HttpServer{
    public:
        typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

        typedef std::function<void(const HttpRequest &,
                                   HttpResponse *)> HttpResponseCallback;
        
        HttpServer(const char *ip, const int port);
        ~HttpServer();

        void HttpDefaultCallBack(const HttpRequest & request, HttpResponse * resp);

        void SetHttpCallback(const HttpResponseCallback &cb);

        void start();

        void onConnection(const TcpConnectionPtr &conn);
        void onMessage(const TcpConnectionPtr &conn);
        void onRequest(const TcpConnectionPtr &conn, const HttpRequest &request);


    private:
        std::unique_ptr<TcpServer> server_;

        HttpResponseCallback response_callback_;
};
