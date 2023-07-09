# day24-定义前端日志库，实现同步输出

在之前的工作中，定义了日志库的输出流。而一个完整的异步日志库系统还需要其前端内容和后端处理。

对于前端的内容，其主要是提供给用户访问日志库的接口，并对日志信息进行一定的格式化，提供给用户将日志信息写入缓冲区的功能。

为了实现该功能，我们创建一个`Logger`类。

为了针对不同的日志等级进行不同的操作，首先需要定义不同的日志等级，通常，日志等级包含如下几个部分:
* DEBUG 指出细粒度信息事件对调试应用程序是非常有帮助的（开发过程中使用）

* INFO 表明消息在粗粒度级别上突出强调应用程序的运行过程。

* WARN 系统能正常运行，但可能会出现潜在错误的情形。

* ERROR 指出虽然发生错误事件，但仍然不影响系统的继续运行。

* FATAL 指出每个严重的错误事件将会导致应用程序的退出。

因此首先定义这几个日志等级。
```c++
class Logger
{
public:
    enum LogLevel
    {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };
}
```

通常，我们希望日志系统包含时间发生的时间，日志等级，发生的事务已经所在的源码位置，例如
```shell
20230703 19:56:51.441099Z347201 INFO  HttpServer Listening on 127.0.0.1:1234 - HttpServer.cpp:31
```
而这些内容通常用户并不关心如何实现。为了方便实现该功能，创建一个`Impl`类，该类主要是对日志信息进行组装，将相应的数据放入到`Buffer`。
```c++
class Impl{
    public:
        DISALLOW_COPY_AND_MOVE(Impl);
        typedef Logger::LogLevel LogLevel;
        Impl(const SourceFile &source, int line, Logger::LogLevel level);
        void FormattedTime();// 格式化时间信息
        void Finish();// 完成格式化，并补充输出源码文件和源码位置

        LogStream &stream();
        const char *loglevel() const;// 获取LogLevel的字符串
        LogLevel level_;// 日志级别

    private:
        Logger::SourceFile sourcefile_; // 源代码名称
        int line_;// 源代码行数
        
        LogStream stream_;// 日志缓存流
};
```
在该类中定义了相应的格式化操作，并在实例化时，我们就直接将相应的信息放入到缓存中。
```c++

Logger::Impl::Impl(const Logger::SourceFile &source, int line, Logger::LogLevel level)
    : sourcefile_(source),
      line_(line),
      level_(level){

    // 格式化时间
    FormattedTime();
    // 输出线程id
    stream_ << std::this_thread::get_id();
    // 日志等级
    stream_ << Template(loglevel(), 6);
}
void Logger::Impl::FormattedTime(){
    //格式化输出时间
    TimeStamp now = TimeStamp::Now();
    time_t seconds = static_cast<time_t>(now.microseconds() / kMicrosecond2Second);
    int microseconds = static_cast<int>(now.microseconds() % kMicrosecond2Second);

    // 变更日志记录的时间，如果不在同一秒，则更新时间。
    // 方便在同一秒内输出多个日志信息
    if (t_lastsecond != seconds) {
        struct tm tm_time;
        localtime_r(&seconds, &tm_time);
        snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d.",
                tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        t_lastsecond = seconds;
    }

    Fmt us(".%06dZ", microseconds);
    stream_ << Template(t_time, 17) << Template(us.data(), 9);
}


void Logger::Impl::Finish(){
    stream_ << "-" << sourcefile_.data_ << ":" << line_ << "\n";
}

```

当前的内容知识将日志的信息进行了组装和放入缓冲区中，而对于实际的日志信息还没有进行任何处理。 我们希望用户在使用该日志库时可以通过简单的操作即可，例如
```c++
LOG_INFO << "LOG Message";
```
通过`LOG_INFO`来规定日志的等级，并输入相应的日志信息。

`MUDUO`通过定义一系列的宏来实现了这一操作，具体的当使用`LOG_*`之类的宏会创建一个临时匿名`Logger`对象，这个对象中包含一个`Impl`对象，而`Impl`对象拥有一个`LogStream`对象。`LOG_*`宏就会返回一个`LogStream`的引用。用于将内容输入到该`LogStream`中的`Buffer`中。

```c++
#define LOG_DEBUG if (loglevel() <= Logger::DEBUG) \
  Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream()
#define LOG_INFO if (loglevel() <= Logger::INFO) \
  Logger(__FILE__, __LINE__, Logger::INFO).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()
```

在析构时，则会将相应的信息输出，如果发生了`FATAL`错误，还会更新缓存区并终止程序。为了设置日志的输出位置，`Logger`定义了两个函数指针用于指定输出位置和更新缓存区。并设置默认输出为`stdout`。
```c++
// Logger.h
typedef void (*OutputFunc)(const char *data, int len); // 定义函数指针
typedef void (*FlushFunc)();
// 默认fwrite到stdout
static void setOutput(OutputFunc);
// 默认fflush到stdout
static void setFlush(FlushFunc);

// Logger.cpp
void defaultOutput(const char* msg, int len){
    fwrite(msg, 1, len, stdout);  // 默认写出到stdout
}
void defaultFlush(){
    fflush(stdout);    // 默认flush到stdout
}
Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;
```

一个完整的析构过程为:
```c++
Logger::~Logger()
{
    impl_.Finish(); // 补足源代码位置和行数
    const LogStream::Buffer& buf(stream().buffer());  // 获取缓冲区
    g_output(buf.data(), buf.len());  // 默认输出到stdout
 
    // 当日志级别为FATAL时，flush设备缓冲区并终止程序
    if (impl_.level_ == FATAL) {
        g_flush();
        abort();
    }
}
```

至此，一个简单的同步日志库基本实现了，其主要流程是，当用户使用一个`LOG_*`的日志宏时，会创建一个临时匿名对象`Logger`，然后`Logger`内部会有一个`Impl`对象，当该对象创建时，会将当前的时间，线程等信息加入到`Buffer`中。之后会该日志宏返回`Impl`中`LogStream`的引用，并将相应的信息输入其拥有的`Buffer`中。当调用结束`Logger`对象被析构时，会调用`g_output`将日志信息输出。并根据不同的日志等级执行不同的操作。


在`HttpServer`和`TcpServer`处增加了两个简单的日志处理，运行