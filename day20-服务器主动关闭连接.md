# day20-服务器主动关闭连接

在上一天实现了一个基础的定时器功能。考虑了半天，我似乎还没有学习到应用到定时器的场景。因此做了一个简单的应用，也就是服务器主动关闭连接，当某个连接连接时间过长时，则服务端将主动关闭连接。

这个功能实现起来并不复杂，首先，在服务端创建连接回调时，加入一个`RunAfter`事务，这个函数将服务端关闭连接的函数传入,当距离上一个上一次活跃时间间隔`autoclosetimeout`时，则尝试关闭连接。

```c++
void HttpServer::onConnection(const TcpConnectionPtr &conn){
    if(auto_close_conn_){
        loop_->RunAfter(AUTOCLOSETIMEOUT, std::move(std::bind(&HttpServer::ActiveCloseConn, this, std::weak_ptr<TcpConnection>(conn))));
    }
}
```

服务端关闭时，将计算当前时间距离当前连接`conn`上一次活跃的时间是否满足定义的自动断开的时长，如果是，则直接关闭，如果不是，则重新加入`RunAfter`事务，我们在主动释放连接时，并不需要去管理`connection`的生命周期，而是简单的使用它，因此采用了`weak_ptr`作为参数。
```c++
void HttpServer::ActiveCloseConn(std::weak_ptr<TcpConnection> & connection){
    TcpConnectionPtr conn = connection.lock(); // 防止conn已经被释放
    if (conn)
    {
        if(TimeStamp::AddTime(conn->timestamp(), AUTOCLOSETIMEOUT) < TimeStamp::Now()){
            conn->Shutdown();
        }else{
            loop_->RunAfter(AUTOCLOSETIMEOUT, std::move(std::bind(&HttpServer::ActiveCloseConn, this, connection)));
        }
    }
}
```
为了保存TCP连接的时长，则为`TcpConnection`增加一个`TimeStamp`成员变量保存当前连接的最后一次活跃时间，并在每次客户端与服务端进行交互时，将`TimeStamp`进行更新。
```c++
void HttpServer::onMessage(const TcpConnectionPtr &conn){
    if (conn->state() == TcpConnection::ConnectionState::Connected)
    {
        if(auto_close_conn_)
            conn->UpdateTimeStamp(TimeStamp::Now());
        // ...
    }
}
```

这只是一个简单的定时器的应用，此外还有许多应用场景，但是并不在本文中赘述。