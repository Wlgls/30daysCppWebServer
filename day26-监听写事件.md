# day26-监听写事件

在`Channel`中，提供了`EnableWrite`函数，但是在整个系统中，并没有对写操作进行监听，服务器对客户端的写入一般直接发生在读事件的过程中。并且会一次性将所有内容写入。尽管保证了数据的完整性，但是性能并不高。

因此我们希望将写事件也注册到`Epoll`中以提高服务器的性能。

由于我们的服务器，`TcpConnetion`的`socket`是`ET`模式的，在`ET`模式下，读事件很容易理解，当TCP缓冲区中从无到有，就会触发，这也意味着，在进行读操作时，必须要将TCP缓冲区中的所有数据一次接收干净，因此很难有再次接收数据的机会。

但是写事件`EPOLLOUT`的触发一般发生在刚刚添加事件或者TCP缓冲区从不可写变成可写时。所以一般不会在创建`socket`时就直接监听读事件(这会导致触发一次`EPOLLOUT`，但是可能没有数据可写，而之后就不再触发了)，而是有程序来控制，具体的来说，当我们调用`Send`函数时，我们会首先发送一次数据，如果此时TCP缓冲区满了导致后续数据没有发送才会注册一个`EPOLLOUT`事件，期待被通知进行下一次发送。


此外，`EPOLLOUT`也可以被强制触发，就是每次在`epoll_ctl`做`EPOLL_CTL_ADD`或者`EPOLL_CTL_MOD`时，如果此时是可写状态，也会被强制触发一次。

利用这两个机制，就可以重新实现服务器的`Send`函数。

在调用Send时，我们首先尝试先发一次数据测试，如果没有将所有数据发送，那么就进行监听写的操作等待后续机会继续发送。
```c++
void TcpConnection::Send(const char *msg, int len){

    int remaining = len;
    int send_size = 0;

    // 如果此时send_buf_中没有数据，则可以先尝试发送数据，
    if (send_buf_->readablebytes() == 0){
        // 强制转换类型，方便remaining操作
        send_size = static_cast<int>(write(connfd_, msg, len));

        if(send_size >= 0){
            // 说明发送了部分数据
            remaining -= send_size;
        }else if((send_size == -1) && 
                    ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
            // 说明此时TCP缓冲区是慢的，没有办法写入，什么都不做
            send_size = 0;// 说明实际上没有发送数据
        }
        else{
            LOG_ERROR << "TcpConnection::Send - TcpConnection Send ERROR";
            return;
        }
    }
    
    // 将剩余的数据加入到send_buf中，等待后续发送。
    assert(remaining <= len);
    if(remaining > 0){
        send_buf_->Append(msg + send_size, remaining);
        // 到达这一步时
        // 1. 还没有监听写事件，在此时进行了监听
        // 2. 监听了写事件，并且已经触发了，此时再次监听，强制触发一次，如果强制触发失败，仍然可以等待后续TCP缓冲区可写。
        channel_->EnableWrite();
    }
}
```

由于我们使用了`Epoll`对写事件进行了监听，那么我们也没有必须一次性的将所有数据进行写完，我们只需要尽可能的将TCP缓冲区写满即可。并将剩余的数据等待下一次写事件被触发即可。

```c++
void TcpConnection::WriteNonBlocking(){

    int remaining = send_buf_->readablebytes();
    int send_size = static_cast<int>(write(connfd_, send_buf_->Peek(), remaining));
    if((send_size == -1) && 
                ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
        // 说明此时TCP缓冲区是满的，没有办法写入，什么都不做 
        // 主要是防止，在Send时write后监听EPOLLOUT，但是TCP缓冲区还是满的，
        send_size = 0; // 在后续`Retrieve`处使用
    }
    else if (send_size == -1){
        LOG_ERROR << "TcpConnection::Send - TcpConnection Send ERROR";
    }

    remaining -= send_size;
    send_buf_->Retrieve(send_size);
}
```

最后，我们在`TcpConnection`的构造函数中，为`Channel`设置写回调即可。

上述修改了`Send`和`WriteNonBlocking`代码，使我们的服务器在面对需要写入大量数据时的情况下不会阻塞在写的操作上。进一步提高服务器的性能。
