# day24-更有效的缓冲区

在过去的操作中，我们使用了`std::string`作为缓冲区的，虽然便于理解，但是由于频繁的操作内存，性能上还是存在一些问题。

另外一个方面就是，整个服务器仍然只有监听读时间，写的操作在读事件发生时立刻被执行了，并且只有当`tcpconnection`中的`send_buffer`中的所有数据都被输出写入之后才会进行下一步，尽管其具有可行性，但是仍然有一些性能上的影响。比如当写的数据量过多，需要多次写操作，那么每次就需要`socket`的写缓冲区清空后再进行写操作，就非常容易阻塞在此处。因此将写事件也放入`Epoll`中显得是有必要的。

那么为了保证`ET`模式写，写数据能够写完，一个解决方法就是如果如果`socket`的写缓冲区还有空间且`send_buffer`的数据还有剩余数据就一直写，当写完之后，就重新注册写事件等待下一次写入。这样就需要能够获得`Buffer`中还有多少数据。之前的操作是在发送数据时将`send_buffer`中的数据加入到一个固定长度的`char`数组中，并设定一个`data_left`来保存每次发送的数据量，来保证每次将`send_buffer`中的数据发送完成。这是可行的，但是如果可以直接在`send_buffer`上获取其剩余的数据量，无疑更加简洁。

考虑到上述内容，我们重新设计了`Buffer`类，这个类由固定长度的`vector<char>`数组进行存储，使用`vector`有一个比较好的好处，就是当我们加入的数据超过`Buffer`的可写空间时，`Buffer`可以自动增长到容纳全部数据。

在程序的内部，我们的`Buffer`的数据结构可以看做三块，即`prependable`, `readable`, `writable`。

```shell
| prependable | readable | writeable |
```

这三个块并不是固定的，而是根据读写操作动态变化的，具体的我们已经写入的数据将被存放在`readable`中，`writable`是剩余的可写空间，随着写入数据增多`readable`将增加，`writable`将降低。而`prependable`则让程序能以很低的代价在数据前面添加几个字节。最简单的例子时，当我们不知道需要增加的消息长度时，可以先进行增加数据，当增加完成后，再在`prependable`前方添加消息的长度。


`Buffer`的定义如下:
```c++
static const int kPrePendIndex = 8; // prependindex长度
static const int kInitalSize = 1024; // 初始化开辟空间长度

class Buffer{
    public:
        DISALLOW_COPY_AND_MOVE(Buffer);

        Buffer();
        ~Buffer();

        // buffer的起始位置
        char *begin();
        // const对象的begin函数，使得const对象调用begin函数时
        // 所得到的迭代器只能对数据进行读操作，而不能进行修改
        const char *begin();

        char *beginread();
        const char *beginread();

        char *beginwrite();
        const char *beginwrite();


        // 添加数据
        void Append(const char *message);
        void Append(const char *message, int len);
        void Append(const std::string &message);


        // 获得可读大小等
        int readablebytes() const;
        int writablebytes() const;
        int prependablebytes() const;

        // 取数据
        // 定长
        void Retrieve(int len);
        std::string RetrieveAsString(int len);

        // 全部
        void RetrieveAll();
        std::string RetrieveAsString();

        // 某个索引之前
        void RetrieveUtil(const char *end);
        std::string RetrieveUtilAsString(const char *end);

        // 查看数据，但是不更新`read_index_`位置
        char *Peek();
        const char *Peek() const;
        std::string PeekAsString(int len);
        std::string PeekAllAsString();

        //查看空间
        void EnsureWritableBytes(int len);

    private:
        std::vector<char> buffer_;
        int read_index_;
        int write_index_;
}
```


在添加消息时，会首先检查当前可写空间是否充足，如果充足的话，就直接写入即可，如果不充足，就需要先进行扩展空间。但是随着不断的从`Buffer`中读取数据，`read_index_`会逐渐后移，导致前方大部分空间被浪费，因此会首先检查`read_index_`前方是否有充足的可写空间，如果充足，就可以先使用这部分空间，而不用额外的开辟内存空间。

```c++
void Buffer::Append(const char* message, int len) {
    EnsureWritableBytes(len);
    std::copy(message, message + len, beginwrite());
    write_index_ += len;
}
void Buffer::EnsureWritableBytes(int len){
    if(writablebytes() >= len)
        return;
    if(writablebytes() + prependablebytes() >= kPrePendIndex + len){
        // 如果此时writable和prepenable的剩余空间超过写的长度，则先将已有数据复制到初始位置，
        // 将不断读导致的read_index_后移使前方没有利用的空间利用上。
        std::copy(beginread(), beginwrite(), begin() + kPrePendIndex);
        write_index_ = kPrePendIndex + readablebytes();
        read_index_ = kPrePendIndex;
    }else{
        buffer_.resize(write_index_ + len);
    }
}
```

从`Buffer`读取数据也非常简单，我们提供了两种读取方式，一种方式是只进行了读取，但是并不改变`read_index_`的位置，也就意味着，这可以重复读取。而另外一种方式则读出后会更改`read_index_`的位置，这意味着，调用这种函数只能读取一次消息。

特别的，我们将读取的数据保存为了字符串形式，以方便后续的处理。

```c++
void Buffer::Retrieve(int len){
    assert(readablebytes() > len);
    if(len + read_index_ < write_index_){
        // 如果读的内容不超过可读空间，则只用更新read_index_
        read_index_ += len;
    }else{
        // 否则就是正好读完，需要同时更新write_index_;
        RetrieveAll();
    }
}

std::string Buffer::RetrieveAsString(int len){
    assert(read_index_ + len <= write_index_);

    std::string ret = std::move(PeekAsString(len));
    Retrieve(len);
    return ret;
}

char *Buffer::Peek() { return beginread(); }
std::string Buffer::PeekAsString(int len){
    return std::string(beginread(), beginread() + len);
}

```

至此，一个简单的缓冲区就实现了，它采用`muduo`的方式，使用`read_index_`和`write_index_`索引将缓冲区的内容分为三块：perpendable、readable、writable。之所以使用索引，而不直接使用指针，主要是为了防止迭代器失效。此外，为了应对不同的应用需求，对`buffer`的读取提供了两种不同的方式。这个缓冲区在一定程度上已经满足了本服务器的需求。后续将把读事件进行监听。

由于本`Buffer`与之前的`Buffer`调用接口发生了变换，因此在`HttpContext`中重载了`ParaseRequest`函数来应对不同的传入参数。尽可能避免对已有函数进行改变。此外，由于还没有进行监听读事件，因此在用到`Buffer`的地方，如`TcpConnection`的`Read`和`Write`操作都需要对函数的调用进行小小的变动。



