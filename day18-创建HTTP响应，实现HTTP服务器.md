# day18-创建HTTP响应，实现HTTP服务器.md

与HTTP请求相似，HTTP响应也存在四个部分分别是状态行，响应头，空行和响应正文

具体的
```
HTTP/版本 状态码 状态描述符
响应头
空行
响应体
```
举例说明
```
HTTP\1.1 200 OK\r\n
Content-Encoding: gzip\r\n
Content-Type: text/html\r\n
Content-Length: 5\r\n
\r\n
hello
```

因此我们只需要根据这个格式创建我们的响应即可，因此创建`HttpResponse`,具体的其保存有响应的信息
```c++
class HttpResponse{
    public:
        HttpResponse(bool close_connection);
        ~HttpResponse();
        void SetStatusCode(HttpStatusCode status_code); // 设置回应码
        void SetStatusMessage(const std::string &status_message);
        void SetCloseConnection(bool close_connection);
        void SetContentType(const std::string &content_type); 
        void AddHeader(const std::string &key, const std::string &value); // 设置回应头
        void SetBody(const std::string &body);
        std::string message(); 
        bool IsCloseConnection();
    private:
        std::map<std::string, std::string> headers_;

        HttpStatusCode status_code_;
        std::string status_message_;
        std::string body_;
        bool close_connection_;
};
```

为了将这些信息整合，以发送至客户端，简单的创建一个`string`来保存这些信息，以备后续使用
```c++
std::string HttpResponse::message(){
    std::string message;
    message += ("HTTP/1.1 " +
                std::to_string(status_code_) + " " +
                status_message_ + "\r\n"
    );
    if(close_connection_){
        message += ("Connection: close\r\n");
    }else{
        message += ("Content-Length: " + std::to_string(body_.size()) + "\r\n");
        message += ("Connection: Keep-Alive\r\n");
    }

    for (const auto&header : headers_){
        message += (header.first + ": " + header.second + "\r\n");
    }

    message += "\r\n";
    message += body_;

    return message;
}
```
自此，我们的HTTP请求和HTTP响应都已经创建完成。为了在建立连接时，能够对HTTP请求进行解析，我们在`TcpConnection`创建一个`HttpContext`，这样，当`TcpConnection`的读缓冲区有信息时，可以将内部消息放到`HttpContext`中进行解析。为了便于实现，在此处实现了每个`TcpConnection`都有一个独有的`HttpContext`,并使用智能指针进行管理。
```c++
// TcpConnection.h
class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
    HttpContext *context() const;
private:
    std::unique_ptr<HttpContext> context_;
}


//TcpConnection.cpp

TcpConnection::TcpConnection(EventLoop *loop, int connfd){
    //...
    context_ = std::make_unique<HttpContext>();
}
HttpContext * TcpConnection::context() const{
    return context_.get();
}
```

随后，我们使用类似于`EchoServer`的方法来创建`HttpServer`，具体的通过设置回调函数的方法，当我们的`TcpConnection`接收到消息时，将读缓冲区中的数据送入到`HttpContext`中解析并构建响应的HTTP响应报文，返回给客户端。

具体的，我们创建`HttpServer`类，该类包含一个`TcpServer`，在创建`HttpServer`时创建`TcpServer`，并传入响应的回调函数，从而间接为为`TcpConnection`创建回调

```c++
HttpServer::HttpServer(const char *ip, const int port)  {
    server_ = std::make_unique<TcpServer>(ip, port);
    server_->set_connection_callback(
        std::bind(&HttpServer::onConnection, this, std::placeholders::_1));

    server_->set_message_callback(
        std::bind(&HttpServer::onMessage, this, std::placeholders::_1)
    );
};
```

我们的`TcpConnection`处理可读信息时，会首先获得自身的`Context`解析器，并尝试解析`read_buf`中的信息，如果解析失败，服务器主动断开连接，否则则对请求报文进行处理并创建响应报文返回。

```c++
void HttpServer::onMessage(const TcpConnectionPtr &conn){
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

```

这样，我们只需要在业务层定义`response_callback`即可。我们以get请求做做了相应的测试

```c++
const std::string html = " <font color=\"red\">This is html!</font> ";
void HttpResponseCallback(const HttpRequest &request, HttpResponse *response)
{
    if(request.method() != HttpRequest::Method::kGet){
        response->SetStatusCode(HttpResponse::HttpStatusCode::k400BadRequest);
        response->SetStatusMessage("Bad Request");
        response->SetCloseConnection(true);
    }

    {
        std::string url = request.url();
        std::cout << url << std::endl;
        if(url == "/"){
            response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
            response->SetBody(html);
            response->SetContentType("text/html");
        }else if(url == "/hello"){
            response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
            response->SetBody("hello world\n");
            response->SetContentType("text/plain");
        }else{
            response->SetStatusCode(HttpResponse::HttpStatusCode::k404NotFound);
            response->SetStatusMessage("Not Found");
            response->SetBody("Sorry Not Found\n");
            response->SetCloseConnection(true);
        }
    }
    return;
}

int main(){
    HttpServer *server = new HttpServer("127.0.0.1", 1234);
    server->SetHttpCallback(HttpResponseCallback);
    server->start();
    return 0;
}
```

通过运行`make http_server`获得响应的可执行文件，然后运行可执行文件，并在任意浏览器中键入`http://127.0.0.1:1234/hello`或`http://127.0.0.1:1234`或其他路径查看服务器的返回情况。
