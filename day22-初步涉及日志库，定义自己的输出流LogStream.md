# day22-初步涉及日志库，定义自己的输出流LogStream

在服务器编程中，日志是必不可少的，生产环境中应做到Log Everything All The Time。

一个日志库需要完成的功能主要有:

* 多种日志级别
* 日志输出目的地为本地文件
* 支持日志文件rooling(按天，按大小)，简化日志归档
* 日志文件命名(进程名+创建日期+创建时间+机器名+进程id+后缀log)
* 日志消息格式固定(日期+时间+线程id+日志级别+源文件名和行号+日志信息)

在之前的操作中，在代码中加入各种信息的输出就是一个简陋的输出端为终端的同步日志系统。如果当我们的服务器发送日志信息后，必须等待日志系统完成写操作才可以继续执行。尽管这个方式可以保证日志数据的完整性和准确率，但是在高并发场景下，会导致服务器的性能下降的非常严重。

而异步日志库在服务器产生日志消息时，会将相应的缓冲区存储起来，等到合适的时机，用一个后台线程统一处理日志信息。这就避免了服务器阻塞在日志系统写操作上，提升服务器的响应性能。

为了存储相应的日志信息，我们需要一个额外的`Buffer`类，这个类与网络端的`Buffer`带有一些不同，在网络端中暂时使用了`std::string`作为存储空间，尽管其非常方便，但是由于其内部是使用动态分配内存的，在频繁的字符串操作中，需要进行内存的动态分配和释放，因此其效率比较低。在日志库中，我们使用了**定长**的字符数组来存储日志信息，可以直接开辟对应的内存空间用于存储信息，在日志库中，将其定义为`FixedBuffer`。
```c++
class FixedBuffer{
    public:
        FixedBuffer();
        ~FixedBuffer();

        void append(const char *buf, int len); // 添加数据

        const char *data() const; // 数据
        int len() const; // 目前的长度

        char *current(); // 获取当前的指针
        int avail() const; // 剩余的可用空间

        void reset(); // 重置缓冲区
        const char *end() const; // 获取末端指针
            
    private:
        char data_[FixedBufferSize];
        char *cur_;
}
```
在这个类中，通过`cur_`保存当前可写内存空间的位置，并在加入新的数据时，直接从该位置写入，并更新`cur_`
```c++
FixedBuffer::FixedBuffer():cur_(data_){};

void FixedBuffer::append(const char *buf, int len){
    if(avail() > len){
        memcpy(cur_, buf, len);
        cur_ += len;
    }
}
```
在日志系统中，并非所有的数据都是字符数据，因此为了进行类型转换，并且采用类似`c++`风格的`stream <<`风格。首先需要定义自己的`LogStream`类，重载`<<`操作符，之所以不直接时用`iostream`是因为，其格式化输出麻烦，并且其操作并不是原子化的。

```c++
class LogStream{
    typedef LogStream self;
    typedef FixedBuffer Buffer;

public:
    self& operator<<(int num)
    self& operator<<(unsigned int num)
    self& operator<<(char v);
    
private:

    Buffer buffer_;
};
```

在实现时，会首先将相应的类别转换成字符形式，然后加入到`buffer`中。
```c++
LogStream &LogStream::operator<<(int num){
    formatInteger(num);
    return *this;
}
LogStream &LogStream::operator<<(const double& num){
    char buf[32];
    int len = snprintf(buf, sizeof(buf), "%g", num);
    buffer_.append(buf, len);
    return *this;
}
```

此外还有其他重载，就不一一赘述了，具体的对于一般类型，都先将其转成字符或者字符串然后加入到`buffer`中，在`muduo`中，针对整形进行了额外的优化，即使用了Matthew wilson设计的旋转除余法进行了转换。

但是`LogStream`本身是并不支持格式化的，因此需要额外的定义一个不影响其状态的`Fmt`类。将一个数值类型数据转换成一个长度不超过32位字符串对象`Fmt`，并重载了支持`Fmt`输出到`LogStream`的`<<`操作符。

在`Fmt`内部，调用了`snprintf`函数，将数据进行了格式化。

```c++
// LogStream.h
template<typename T>
Fmt::Fmt(const char* fmt, T val)
{
  static_assert(std::is_arithmetic<T>::value == true, "Must be arithmetic type");
  length_ = snprintf(buf_, sizeof(buf_), fmt, val);
  assert(static_cast<size_t>(length_) < sizeof(buf_));
}

inline LogStream & operator<<(LogStream& s, const Fmt& fmt){
    s.append(fmt.data(), fmt.length());
    return s;
}
```

一个简单的流式`LogStream`就简单的实现了，在`/test/test_logstream.cpp`对其进行了简单的测试。
进入`build`文件，并运行`make test_logstream`会生成相应的可执行文件，执行他就可以进行简单的测试了。