/*
Buffer的实现

简单的buffer无法满足http的需求，因此，采用一种更为高效的buffer方法

在这个buffer中，期望系统调用的次数少，内存的占用少。

内存存储采用了vector。这样内存的扩充直接由stl控制。



*/

#pragma once
#include <string>

class Buffer {
 public:
  Buffer() = default;
  ~Buffer() = default;

  void Append(const char *_str, int _size);
  ssize_t Size();
  const char *ToStr();
  void Clear();
  void Getline();
  void SetBuf(const char *buf);

 private:
  std::string buf_;
};
