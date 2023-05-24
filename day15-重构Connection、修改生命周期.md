# day15-重构Conecction、修改生命周期

在昨天的重构中，将每个类独属的资源使用`unique_ptr`进行了包装。但是对于`TcpConnection`这个类，其生命周期模糊，使用`unique_ptr`很容易导致内存泄漏。这是因为我们的对于`TcpConnection`是被动关闭，当我们`channel`在`handleEvent`时，发现客户端传入了关闭连接的信息后，直接对`onClose`进行了调用。因此如果使用`unqiue_ptr`时，我们在调用`onclose`时已经销毁了`tcpconnection`，而对应的channel也会被移除，但是此时的`HandleEvent`并没有结束，因此存在了内存泄漏。

为了解决这个问题，采用了`shared_ptr`去管理`TcpConnection`.方便延长`TcpConnection`的生命周期。从上面的分析可以看出，只需要使我们的`TcpConnection`的生命周期长于`HandleEvent`即可。

为了达到这个目的，在channel类中增加一个`weak_ptr`来引用这个`Tcpconnection`。并在`HandleEvent`时暂时的提升`TcpConnection`的引用计数。

具体的，在channel中增加一个`weak_ptr`，并在`HandleEvent`时暂时将其计数+1.

```c++
// Channel.h
class Channel{
public:
    void HandleEvent() const;
    void HandleEventWithGuard() const;
    void Tie(const std::shared_ptr<void> &ptr);
private:
    std::weak_ptr<void> tie_;
}
//Channel.cpp
void Channel::HandleEvent() const{
    if(tie_){
        std::shared_ptr<void> guard() = tie_.lock();
        HandleEventWithGuard();
    }else{
        HandleEventWithGuard();
    }
}
void Channel::HandleEventWithGuard() const{
    if(ready_events_ & READ_EVENT){
        read_callback_();
    }
    if(ready_events_ & WRITE_EVENT){
        write_callback_();
    }
}
```

为了解决这个问题，采用了`share_ptr`去管理`TcpConnection`。首先将`TcpConnection`继承自`enable_shared_from_this`并在关闭和连接时传入`share_ptr`。除此之外，我们还需要在建立`TcpConnection`时将其绑定到`Channel::tie_`上。

```C++
// TcpConnection.h
class TcpConnection : public  std::enable_shared_from_this<TcpConnection>{
    public:
    	void ConnectionEstablished();
    	void set_connection_callback(std::function<void(const std::shared_ptr<TcpConnection> &)> const &fn);
   		void set_message_callback(std::function<void(const std::shared_ptr<TcpConnection> &)> const &fn); 
    private:
    	std::function<void(const std::shared_ptr<TcpConnection> &)> on_message_;
    	std::function<void(const std::shared_ptr<TcpConnection> &)> on_connect_;
}
// TcpConneciton.cpp
TcpConnection::TcpConnection(EventLoop *loop, int connfd){
    socket_ = std::make_unique<Socket>();
    socket_->SetFd(connfd);
    if(loop != nullptr){
        channel_ = std::make_unique<Channel>(connfd, loop);
        channel_->EnableRead();
        channel_->EnableET();
    }
    read_buf_ = std::make_unique<Buffer>();
    send_buf_ = std::make_unique<Buffer>();
}

void TcpConnection::ConnectionEstablished(){
    state_ = ConnectionState::Connected;
    channel_->Tie(shared_from_this());
    if (on_connect_){
        on_connect_(shared_from_this());
    }
}

```

因此我们的`TcpServer`也需要进行相应的变动，所有关闭`TcpConnection`都需要相应的改变成`share_ptr`。而在`TcpServer`在建立连接时，需要调用`ConnectionEstablished`来保证`TcpConnection`与`tie_`绑定。

```c++
// TcpServer.h	
class TcpServer{
    public:
    	void set_connection_callback(std::function < void(const std::shared_ptr<TcpConnection> &)> const &fn);
    	void set_message_callback(std::function < void(const std::shared_ptr<TcpConnection> &)> const &fn);
    private:
    	std::unordered_map<int, std::shared_ptr<TcpConnection>> connectionsMap_;
    	std::function<void(const std::shared_ptr<TcpConnection> &)> on_connect_;
        std::function<void(const std::shared_ptr<TcpConnection> &)> on_message_;
}

// TcpServer.cpp
RC TcpServer::OnNewConnection(int fd){
    assert(fd != -1);
    uint64_t random = fd % sub_reactors_.size();

    std::shared_ptr<TcpConnection> conn = std::make_shared<TcpConnection>(sub_reactors_[random].get(), fd);
    std::function<void(int)> cb = std::bind(&TcpServer::OnClose, this, std::placeholders::_1);
    conn->set_connection_callback(on_connect_);
    conn->ConnectionEstablished();
    conn->set_close_callback(cb);
    conn->set_message_callback(on_message_);
    connectionsMap_[fd] = std::move(conn);
    return RC_SUCCESS;
}

```

通过上述操作，保证了`TcpConnection`的生命周期与`HandleEvent`相同。具体上讲

在连接到来时，创建`TcpConnection`,并用`shared_ptr`管理，此时引用计数为1.

而当`HandleEvent`时，将`tie_`提升，得到一个`shared_ptr`，引用计数为2。

当关闭时，`TcpServer`中`erase`后`TcpConnection`引用计数变为了1。而`HandleEvent`之后完之后引用计数变为了0。自动销毁。

