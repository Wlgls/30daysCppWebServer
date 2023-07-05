# 30天自制C++服务器

[源仓库地址](https://github.com/yuesong-feng/30dayMakeCppServer/)

本文主要是狗尾续貂，对原仓库的后续工作进行了简单的处理，由于本人能力有限，在原仓库的基础上，抛弃了day13的工程定义，而专注于功能的实现，并进行了自我改造，前day01至day12与源仓库相同，后续为相应的改编。

将原有的内容扩展到了日志库部分，但是只完成了同步日志库。后续应该不会进行更加深入的修改。主要在于随着内容的深入，贫乏的知识储备量越来越不够了，另外由于整个项目前期存在很严重的问题，如生命周期的管理等，导致后续问题难以定位。

在我看来，推翻重做要比重新做要更方便一些。不过其本身就是对`muduo`的解析，其历史价值还是被完成了的。后续在进行更改。

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

[day14-重构服务器、使用智能指针](https://github.com/Wlgls/30daysCppWebServer/blob/main/day14-重构服务器、使用智能指针.md)

[day15-重构Connection、修改生命周期](https://github.com/Wlgls/30daysCppWebServer/blob/main/day15-重构Connection、修改生命周期.md)

[day16-HTTP有限状态转换机](https://github.com/Wlgls/30daysCppWebServer/blob/main/day16-HTTP有限状态转换机.md)

[day17-创建HTTP响应，实现HTTP服务器](https://github.com/Wlgls/30daysCppWebServer/blob/main/day17-创建HTTP响应，实现HTTP服务器.md)

[day18-去除Socket类、简化代码](https://github.com/Wlgls/30daysCppWebServer/blob/main/day18-去除Socket类、简化代码.md)

[day20-使用CMake工程化](https://github.com/Wlgls/30daysCppWebServer/blob/main/day20-使用CMake工程化.md)

[day21-定时器的创建使用](https://github.com/Wlgls/30daysCppWebServer/blob/main/day21-定时器的创建使用.md)

[day22-服务器主动关闭连接](https://github.com/Wlgls/30daysCppWebServer/blob/main/day22-服务器主动关闭连接.md)

[day23-初步涉及日志库，定义自己的输出流LogStream](https://github.com/Wlgls/30daysCppWebServer/blob/main/day23-初步涉及日志库，定义自己的输出流LogStream.md)

[day24-定义前端日志库，实现同步输出](https://github.com/Wlgls/30daysCppWebServer/blob/main/day24-定义前端日志库，实现同步输出.md)

