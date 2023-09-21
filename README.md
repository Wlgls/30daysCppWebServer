# 30天自制C++服务器

[源仓库地址](https://github.com/yuesong-feng/30dayMakeCppServer/)

本文主要是狗尾续貂，对原仓库的后续工作进行了简单的处理，由于本人能力有限，在原仓库的基础上专注于功能的实现，并进行了自我改造，前day01至day12与源仓库相同，后续为相应的改编。

现在大致上就已经完成了该内容，在原有的基础上补足了HTTP协议的解析，异步日日志库模块和定时器模块。并使用文件上传和下载进行了简单的应用。由于水平不足，里面可能充斥着各种各样的问题，但是确实一点点仿照着原仓库整完了，关于文件的上传和下载，可以说写的非常简陋，里面bug应该很多，应用场景也几乎没有。后续可能还需要进行增加和修订，但是现在还需要增加理论基础。

在进行编写时，可以重新创建一个仓库，并在当前教程进行`commit`后，将后一个教程进行覆盖以查看下一个教程所进行的修改。

[day01-从一个最简单的socket开始](https://github.com/Wlgls/30daysCppWebServer/blob/main/day01-从一个最简单的socket开始.md)

[day02-不要放过任何一个错误](https://github.com/Wlgls/30daysCppWebServer/blob/main/day02-不要放过任何一个错误.md)

[day03-高并发还得用epoll](https://github.com/Wlgls/30daysCppWebServer/blob/main/day03-高并发还得用epoll.md)

[day04-来看看我们的第一个类](https://github.com/Wlgls/30daysCppWebServer/blob/main/day04-来看看我们的第一个类.md)

[day05-epoll高级用法-Channel登场](https://github.com/Wlgls/30daysCppWebServer/blob/main/day05-epoll高级用法-Channel登场.md)

[day06-服务器与事件驱动核心类登场](https://github.com/Wlgls/30daysCppWebServer/blob/main/day06-服务器与事件驱动核心类登场.md)

[day07-为我们的服务器添加一个Acceptor](https://github.com/Wlgls/30daysWebCppServer/blob/main/day07-为我们的服务器添加一个Acceptor.md)

[day08-一切皆是类，连TCP连接也不例外](https://github.com/Wlgls/30daysCppWebServer/blob/main/day08-一切皆是类，连TCP连接也不例外.md)

[day09-缓冲区-大作用](https://github.com/Wlgls/30daysCppWebServer/blob/main/day09-缓冲区-大作用.md)

[day10-加入线程池到服务器](https://github.com/Wlgls/30daysCppWebServer/blob/main/day10-加入线程池到服务器.md)

[day11-完善线程池，加入一个简单的测试程序](https://github.com/Wlgls/30daysCppWebServer/blob/main/day11-完善线程池，加入一个简单的测试程序.md)

[day12-将服务器改写为主从Reactor多线程模式](https://github.com/Wlgls/30daysCppWebServer/blob/main/day12-将服务器改写为主从Reactor多线程模式.md)

[day13-支持业务逻辑自定义、完善Connection类](https://github.com/Wlgls/30daysCppWebServer/blob/main/day13-支持业务逻辑自定义、完善Connection类.md)

[day14-重构核心库、使用智能指针](https://github.com/Wlgls/30daysCppWebServer/blob/main/day13-重构核心库、使用智能指针.md)

[day15-重构Connection、修改生命周期](https://github.com/Wlgls/30daysCppWebServer/blob/main/day15-重构Connection、修改生命周期.md)

[day16-使用CMake工程化](https://github.com/Wlgls/30daysCppWebServer/blob/main/day16-使用CMake工程化.md)

[day17-使用EventLoopThreadPool、移交EventLoop](https://github.com/Wlgls/30daysCppWebServer/blob/main/day17-使用EventLoopThreadPool、移交EventLoop.md)

[day18-HTTP有限状态转换机](https://github.com/Wlgls/30daysCppWebServer/blob/main/day18-HTTP有限状态转换机.md)

[day19-创建HTTP响应，实现HTTP服务器](https://github.com/Wlgls/30daysCppWebServer/blob/main/day19-创建HTTP响应，实现HTTP服务器.md)

[day20-定时器的创建使用](https://github.com/Wlgls/30daysCppWebServer/blob/main/day20-定时器的创建使用.md)

[day21-服务器主动关闭连接](https://github.com/Wlgls/30daysCppWebServer/blob/main/day21-服务器主动关闭连接.md)

[day22-初步涉及日志库，定义自己的输出流LogStream](https://github.com/Wlgls/30daysCppWebServer/blob/main/day22-初步涉及日志库，定义自己的输出流LogStream.md)

[day23-定义前端日志库，实现同步输出](https://github.com/Wlgls/30daysCppWebServer/blob/main/day23-定义前端日志库，实现同步输出.md)

[day24-异步日志库](https://github.com/Wlgls/30daysCppWebServer/blob/main/day24-异步日志库.md)

[day25-更有效的缓冲区](https://github.com/Wlgls/30daysCppWebServer/blob/main/day25-更有效的缓冲区.md)

[day26-监听写事件](https://github.com/Wlgls/30daysCppWebServer/blob/main/day26-监听写事件.md)

[day27-处理静态文件，实现POST请求](https://github.com/Wlgls/30daysCppWebServer/blob/main/day27-处理静态文件，实现POST请求.md)

[day28-文件服务器的简单实现，文件的展示和下载](https://github.com/Wlgls/30daysCppWebServer/blob/main/day28-文件服务器的简单实现，文件的展示和下载.md)

[day29-文件的上传](https://github.com/Wlgls/30daysCppWebServer/blob/main/day29-文件的上传.md)

[day30-WebBench的测试](https://github.com/Wlgls/30daysCppWebServer/blob/main/day30-WebBench的测试.md)

