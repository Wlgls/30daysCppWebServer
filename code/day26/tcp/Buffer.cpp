#include "Buffer.h"
#include <string>
#include <assert.h>
#include <cstring>
Buffer::Buffer()
    : buffer_(kInitalSize),
      read_index_(kPrePendIndex),
      write_index_(kPrePendIndex){}

Buffer::~Buffer(){}

char *Buffer::begin() { return &*buffer_.begin(); }
const char *Buffer::begin() const { return &*buffer_.begin(); }
char* Buffer::beginread() { return begin() + read_index_; } 
const char* Buffer::beginread() const { return begin() + read_index_; }
char* Buffer::beginwrite() { return begin() + write_index_; }
const char* Buffer::beginwrite() const { return begin() + write_index_; }

void Buffer::Append(const char* message) {
    Append(message, static_cast<int>(strlen(message)));
}

void Buffer::Append(const char* message, int len) {
    EnsureWritableBytes(len);
    std::copy(message, message + len, beginwrite());
    write_index_ += len;
}

void Buffer::Append(const std::string& message) {
    Append(message.data(), static_cast<int>(message.size())); 
}


int Buffer::readablebytes() const { return write_index_ - read_index_; }
int Buffer::writablebytes() const { return static_cast<int>(buffer_.size()) - write_index_; } 
int Buffer::prependablebytes() const { return read_index_; }


std::string Buffer::PeekAsString(int len){
    return std::string(beginread(), beginread() + len);
}

std::string Buffer::PeekAllAsString(){
    return std::string(beginread(), beginwrite());
}

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

void Buffer::RetrieveAll(){
    write_index_ = kPrePendIndex;
    read_index_ = write_index_;
}

void Buffer::RetrieveUtil(const char *end){
    // 保证没有进入可写部分
    assert(beginwrite() >= end);
    read_index_ += static_cast<int>(end - beginread());
}

std::string Buffer::RetrieveAsString(int len){
    assert(read_index_ + len <= write_index_);

    std::string ret = std::move(PeekAsString(len));
    Retrieve(len);
    return ret;
}

std::string Buffer::RetrieveUtilAsString(const char *end){
    assert(beginwrite() >= end);
    std::string ret = std::move(PeekAsString(static_cast<int>(end - beginread())));
    RetrieveUtil(end);
    return ret;
}

std::string Buffer::RetrieveAllAsString(){
    assert(readablebytes() > 0);
    std::string ret = std::move(PeekAllAsString());
    RetrieveAll();
    return ret;
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
