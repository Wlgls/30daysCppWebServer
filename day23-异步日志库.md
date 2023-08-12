# day23-异步日志库

在实现同步日志库时，我们设定了`Logger`的输出和刷新为`stdout`，而异步日志库主要是将`Logger`的输出交由了后端程序来处理。

对于异步日志库的实现，主要是通过准备两块缓冲区，前端负责往buffer A中写日志消息，后端负责将buffer B中的日志消息写入磁盘文件。当buffer A写满之后，后端线程中会交换buffer A和buffer B，让前端往buffer B中写入日志消息，后端将buffer A中的日志消息写到磁盘文件中，如此往复。同时，为了及时将生成的日志消息写入文件，便于管理者分析日志消息，即使buffer A未满，日志库也会每3秒执行交换写入操作。这就避免了前端每生成一条日志消息就传送给后端，而是将多条日志消息拼成一个大buffer传送给后端线程，相当于批量处理，减少了后端线程的唤醒频率，降低了服务器开销。

这明显是一个生产者消费者模式，只有当我们的后端线程创建成功之后，我们才可以不断的往`Buffer A`中写入数据，否则当`Buffer A`已满，我们无法将其写入到磁盘文件中，可能会有丢失信息的问题。为了保证他的线程安全，需要使用`Latch`机制，其机制对于线程同步机制来说很简单，主要针对一个线程等待另一个或多个线程，其内部实现就是一些线程能够等待直到计数器变为零。但是`std::latch`在`c++20`才被引入，本次并不使用该方法，而是定义一个`Latch`类来实现该机制。

在这个类中有一个`count_`变量，构造时给定一个初值，代表需要等待的线程数。每个线程完成一个任务，`count_`减1，当`count_`值减到0时，代表所有线程已经完成了所有任务，在`Latch`上等待的线程就可以继续执行了。

```c++
class Latch
{
private:
    std::mutex mux_;
    std::condition_variable cv_;
    int count_;

public:
    DISALLOW_COPY_AND_MOVE(Latch);

    explicit Latch(int count) : count_(count){}
    void wait(){
        std::unique_lock<std::mutex> lock(mux_);
        while(count_ > 0){
            cv_.wait(lock);
        }
    }

    void notify(){
        std::unique_lock<std::mutex> lock(mux_);
        --count_;
        if(count_ == 0){
            cv_.notify_all();
        }
    }
};
```

上述的操作只是定义了一个`Latch`类，当然如果使用`c++20`的话，也可以直接使用`std::latch`来进行管理。

而对于日志库的主要逻辑而言，其关键的工作在于，
1. 如何往buffer A中写入日志信息。
2. 后端如何操作并将相应的信息写入磁盘文件。

在同步日志库中，我们设定了`Logger`的输出和刷新都是`stdout`。我们只要将其变为往`Buffer A`中添加数据即可。
为了实现这个目的，定义了一个`AsyncLogging`。
```c++
class AsyncLogging{
    public:
        typedef FixedBuffer<FixedLargeBuffferSize> Buffer;

        AsyncLogging(const char *filepath = nullptr);
        ~AsyncLogging();

        void Stop();
        void Start();

        void Append(const char* data, int len);
        void Flush();
        void ThreadFunc();
    
    private:
        
        bool running_;
        const char *filepath_;

        // 线程相关
        std::mutex mutex_;
        std::condition_variable cv_;
        Latch latch_;
        std::thread thread_;

        std::unique_ptr<Buffer> current_; // 当前的缓存
        std::unique_ptr<Buffer> next_; // 预备的缓冲
        std::vector<std::unique_ptr<Buffer>> buffers_;// 已满的缓冲区
};
```

在这个类中拥有两个`Buffer`成员变量。主要是为了防止当一次性产生大量日志信息时，单个buffer无法及时的保存所有的信息，`next_`作为预备役被定义。

在添加数据时，我们会判断`current_`剩余空间是否充足，如果不充足，就可以往第一个缓冲区中添加数据，如果第二个也被使用了，就需要重新开辟内存空间，但是这种情况是极少的，因此对运行的效率的影响应该是微乎其微。

此外，还有一个`Buffer`的`vector`数组，这其实就相当于异步日志库中的`Buffer A`。
```c++
// AsyncLogging.cpp
void AsyncLogging::Append(const char *data, int len){
    std::unique_lock<std::mutex> lock(mutex_);
    if(current_->avail() >= len){
        current_->append(data, len);
    }else{
        // 如果当前缓存没有空间，就将当前缓存放入到已满列表中
        buffers_.push_back(std::move(current_));
        if (next_){ // 如果预备缓冲区未被使用，则
            current_ = std::move(next_);
        }else{
            current_.reset(new Buffer());
        }
        // 向新的缓冲区写入信息。
        current_->append(data, len);
    }
    // 唤醒后端线程
    cv_.notify_one();
}
```
在之后，我们就可以将该函数作为`Logger`的输出对象，这就保证了`Logger`产生的日志信息将存放在`Buffer A`中。例如：
```c++
// test_httpserver.cpp
std::unique_ptr<AsyncLogging> asynclog;
// 由于`Append`不是静态函数，所以需要先声明一个实例对象。
void AsyncOutputFunc(const char *data, int len)
{
    asynclog->Append(data, len);
}
int main(){
    asynclog = std::make_unique<AsyncLogging>();
    Logger::setOutput(AsyncOutputFunc);
}

```
同理,`Flush`也可以进行相同的操作。

对于后端线程的建立，其主要就是对`Buffer A`和`Buffer B`的交换，并写入日志文件中，在后端线程中，会额外创建一个存储`Buffer`的`Vector`，作为我们的`Buffer B`.

其余的操作并不关键，只是增加运行的效率，例如在线程开始时开辟两个`Buffer`空间后续直接分配给主线程的`current_`和`next_`就省去了主线程重新开辟空间的时间，在最后保留两个已满`Buffer`不释放，而是清空并分配也是为了减少重新开辟空间的次数。
```c++
void AsyncLogging::ThreadFunc(){
    // 创建成功，提醒主线程
    latch_.notify();

    std::unique_ptr<Buffer> new_current = std::make_unique<Buffer>();
    std::unique_ptr<Buffer> new_next = std::make_unique<Buffer>();


    std::unique_ptr<LogFile> logfile = std::make_unique<LogFile>();

    new_current->bzero();
    new_next->bzero();

    std::vector<std::unique_ptr<Buffer>> active_buffers;
    
    while(running_){
        std::unique_lock<std::mutex> lock(mutex_);
        if(buffers_.empty()){
            // 如果还没有已满缓冲区，则等待片刻
            cv_.wait_until(lock, std::chrono::system_clock::now() + BufferWriteTimeout * std::chrono::milliseconds(1000),
                          []{ return false; });
        }

        // 直接将当前的缓冲区看错已满缓冲区中，减少操作。
        buffers_.push_back(std::move(current_));

        // Buffer A与Buffer B交换
        active_buffers.swap(buffers_);

        current_ = std::move(new_current);

        if(!next_){
            next_ = std::move(new_next);
        }

        // 写入日志文件
        for (const auto & buffer: active_buffers){
            logfile->Write(buffer->data(), buffer->len());
        }

        if(logfile->writtenbytes() >= FileMaximumSize){
            // 如果文件已写内容超过最大空间，新建一个。
            logfile.reset(new LogFile(filepath_));
        }

        if (active_buffers.size() > 2)
        {
            // 留两个，用户后续直接分配，不需要再额外的进行开辟内存空间，增加效率。
            active_buffers.resize(2);
        }

        if(!new_current){
            new_current = std::move(active_buffers.back());
            active_buffers.pop_back();
            new_current->bzero();
        }
        if(!new_next){
            new_next = std::move(active_buffers.back());
            active_buffers.pop_back();
            new_next->bzero();
        }

        active_buffers.clear();
    }
    
}
```

为了保证首先创建了日志库的后端线程，因此在此处采用了`Latch`机制，在`AsyncLogging`处定义了一个`Latch`变量，因为一般一个服务器只有一个后端日志线程，因此初始化`count`为1, 当该线程被创建时，`count`变为0，前端线程可以继续执行。

具体的其被定义在
```c++
// AsyncLogging.cpp
void AsyncLogging::Start(){
    running_ = true;
    thread_ = std::thread(std::bind(&AsyncLogging::ThreadFunc, this));

    // 等待线程启动完成。
    latch_.wait();
}
```

上述就是一个简单的异步日志库的实现，上述代码中还提到了`LogFile`,其内部实现非常简单,只是打开文件,并定义了一个写入文件的函数,此处就不再赘述。

至此，一个完整的日志库就实现了。在本教程中，为了实现前端、后端的异步操作，同时避免前端每次生成日志消息都唤醒后端线程，提高日志处理效率，采用的是双缓冲技术，具体的思想就是：准备两块缓冲区，前端负责往buffer A中写日志消息，后端负责将buffer B中的日志消息写入磁盘文件。当buffer A写满之后，后端线程中会交换buffer A和buffer B，让前端往buffer B中写入日志消息，后端将buffer A中的日志消息写到磁盘文件中，如此往复。这就避免了前端每生成一条日志消息就传送给后端，而是将多条日志消息拼成一个大buffer传送给后端线程，相当于批量处理，减少了后端线程的唤醒频率，降低了服务器开销。

除此之外，我们还定义了`Latch`类用于处理线程同步，并将`FixedBuffer`类修改为了模板类来处理前端和后端对不同`Buffer`大小的需求。

在`test_httpserver.cpp`中添加了`异步日志库`的代码，通过之前的操作编译连接并运行它即可测试异步日志库的效果。需要注意的是，需要首先创建一个`LogFiles`文件夹用于存储日志文件，这是因为`fopen`并不会根据路径创建文件夹。