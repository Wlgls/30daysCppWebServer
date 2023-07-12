#include "Logging.h"
#include "CurrentThread.h"
#include <utility>

// 为了实现多线程中日志时间格式化的效率，增加了两个__thread变量，
// 用于缓存当前线程存日期时间字符串、上一次日志记录的秒数
__thread char t_time[64];		// 当前线程的时间字符串 “年:月:日 时:分:秒”
__thread time_t t_lastsecond;	// 当前线程上一次日志记录时的秒数

// 方便一个已知长度的字符串被送入buffer中
class Template{
    public:
        Template(const char * str, unsigned len)
            : str_(str),
              len_(len){}
        const char *str_;
        const unsigned len_;
};

// 重载运算符，使LogStream可以处理Template类型的数据。
inline LogStream& operator<<(LogStream& s, Template v){
    s.append(v.str_, v.len_);
    return s;

}
// 重载运算符，使LogStream可以处理SourceFile类型的数据
inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& v){
    s.append(v.data_, v.size_);
    return s;
}


Logger::SourceFile::SourceFile(const char *data) : data_(data), size_(static_cast<int>(strlen(data_)))
{
    const char *forward_slash = strrchr(data, '/');
    if (forward_slash)
    {
        data_ = forward_slash + 1;
        size_ -= static_cast<int>((data_ - data));
    }
}

Logger::Impl::Impl(Logger::LogLevel level, const Logger::SourceFile &source, int line)
    : level_(level),
      sourcefile_(source),
      line_(line){

    FormattedTime();
    CurrentThread::tid();

    stream_ << Template(CurrentThread::tidString(), CurrentThread::tidStringLength());
    stream_ << Template(loglevel(), 6);
}

void Logger::Impl::FormattedTime(){
    //格式化输出时间
    TimeStamp now = TimeStamp::Now();
    time_t seconds = static_cast<time_t>(now.microseconds() / kMicrosecond2Second);
    int microseconds = static_cast<int>(now.microseconds() % kMicrosecond2Second);

    // 变更日志记录的时间，如果不在同一秒，则更新时间。

    if (t_lastsecond != seconds)
    {
        struct tm tm_time;
        localtime_r(&seconds, &tm_time);
        snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d.",
                tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        t_lastsecond = seconds;
    }

    Fmt us(".%06dZ  ", microseconds);
    stream_ << Template(t_time, 17) << Template(us.data(), 9);
}

void Logger::Impl::Finish(){
    stream_ << " - " << sourcefile_.data_ << ":" << line_ << "\n";
}

LogStream &Logger::Impl::stream() { return stream_; }

const char* Logger::Impl::loglevel() const {
    switch(level_) {
        case DEBUG:
            return "DEBUG ";
        case INFO:
            return "INFO  ";
        case WARN:
            return "WARN  ";
        case ERROR:
            return "ERROR ";
        case FATAL:
            return "FATAL ";
        }   
    return nullptr;
} 


void defaultOutput(const char* msg, int len){
    fwrite(msg, 1, len, stdout);  // 默认写出到stdout
}

void defaultFlush(){
    // 强制刷新输出流缓冲区
    fflush(stdout);    // 默认flush到stdout
}

// 定义默认值
Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;
Logger::LogLevel g_logLevel = Logger::LogLevel::INFO;

Logger::Logger(const char *file_, int line, Logger::LogLevel level)
    : impl_(level, file_, line){};


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


LogStream &Logger::stream() { return impl_.stream(); }

void Logger::setOutput(Logger::OutputFunc func){
    g_output = func;
}

void Logger::setFlush(Logger::FlushFunc func){
    g_flush = func;
}

void Logger::setLogLevel(Logger::LogLevel level){
    g_logLevel = level;
}