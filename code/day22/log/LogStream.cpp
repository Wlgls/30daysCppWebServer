#include <stdio.h>
#include "LogStream.h"

FixedBuffer::FixedBuffer():cur_(data_){};
FixedBuffer::~FixedBuffer(){};

void FixedBuffer::append(const char *buf, int len){
    if(avail() > len){
        memcpy(cur_, buf, len);
        cur_ += len;
    }
}

const char *FixedBuffer::data() const { return data_; }
int FixedBuffer::len() const { return static_cast<int>(cur_ - data_); }

char *FixedBuffer::current() { return cur_; }
int FixedBuffer::avail() const { return static_cast<int>(end() - cur_); }
void FixedBuffer::add(int len) { cur_ += len; }

void FixedBuffer::reset() { cur_ = data_; }
void FixedBuffer::clear() { bzero(data_, sizeof(data_)); }
const char *FixedBuffer::end() const { return data_ + sizeof(data_); }

LogStream::LogStream(){}
LogStream::~LogStream(){}

void LogStream::append(const char *data, int len){
    buffer_.append(data, len);
}

const LogStream::Buffer & LogStream::buffer() const { return buffer_; }

void LogStream::resetBuffer() {
    buffer_.clear();
    buffer_.reset();
}


LogStream& LogStream:: operator<<(bool v){
    buffer_.append(v ? "1" : "0", 1);
    return *this;
}
// 整形数据的字符串转换、保存到缓冲区； 内部均调用 void formatInteger(T); 
LogStream& LogStream:: operator<<(short num){
    return (*this) << static_cast<int>(num);
}
LogStream& LogStream:: operator<<(unsigned short num){
    return (*this) << static_cast<unsigned int>(num);
}
LogStream& LogStream:: operator<<(int num){
    formatInteger(num);
    return *this;
}
LogStream& LogStream:: operator<<(unsigned int num){
    formatInteger(num);
    return *this;
}
LogStream& LogStream:: operator<<(long num){
    formatInteger(num);
    return *this;
}
LogStream& LogStream:: operator<<(unsigned long num){
    formatInteger(num);
    return *this;
}
LogStream& LogStream:: operator<<(long long num){
    formatInteger(num);
    return *this;
}
LogStream& LogStream:: operator<<(unsigned long long num){
    formatInteger(num);
    return *this;
}

// 浮点类型数据转换成字符串
LogStream& LogStream:: operator<<(const float & num){
    return (*this) << static_cast<const double>(num);
}

LogStream& LogStream:: operator<<(const double& num){
    char buf[32];
    int len = snprintf(buf, sizeof(buf), "%g", num);
    buffer_.append(buf, len);
    return *this;
}

LogStream& LogStream:: operator<<(char v){
    buffer_.append(&v, 1);
    return *this;
}

// 原生字符串输出到缓冲区
LogStream& LogStream:: operator<<(const char* str){
    if (str){ buffer_.append(str, strlen(str)); }
    else    { buffer_.append("(null)", 6);      }

    return *this;
}

// 标准字符串std::string输出到缓冲区
LogStream& LogStream:: operator<<(const std::string& v){
    buffer_.append(v.c_str(), v.size());
    return *this;
}