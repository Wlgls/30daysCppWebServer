# day18-去除Socket类、简化代码


在进行debug过程中，`Socket`可以认为是一个冗余的，他的功能完全可以集成在`Acceptor`中，其内部实现的功能完全是服务于`Acceptor`的，因此将其集成在`Acceptor`中，也方便更好的管理
```c++
class Acceptor
{
public:

    RC Bind(const char *ip, const int port) const;
    RC Create();
    RC Listen() const;
    RC Accept(int &clnt_fd) const;
}
```


除此之外，不需要像原博主对MACOS的适配，因此对`Epoller`也进行了修改，以更好的观察监听事物的变化。

需要注意的是，当前版本仍然存在许多问题，尤其是在进行webbench压测时，总是内存泄漏，暂时无法定位原因，不知道是不是没有严格按照muduo去管理TcpConnection.