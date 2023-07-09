# day20-使用CMake工程化

最开始只想实现功能，而忽视其他，但是随着文件越来越多，项目的管理就愈加繁杂，因此最终还是编写了简单的`CMakeLists.txt`进行管理

在当前的版本中，一共主要有三个文件夹，`base`文件夹存放基础的组件，`http`存放对HTTP协议的解析和服务器的构建，`tcp`存放TCP的地层文件
```c++
include_directories(
    base
    tcp
    http
)
```

在编译，可以根据不同的环境设置不同的编译参数：
```c++
SET(CMAKE_CXX_FLAGS "-g -Wall -Werror -std=c++14")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
```

最后我们只需要收集对应目录下的所有源文件并编译连接即可
```c++
aux_source_directory(base SRC_LIST1)
aux_source_directory(tcp SRC_LIST2)
aux_source_directory(http SRC_LIST3)



set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/test/)

add_executable(WebServer http_server.cpp ${SRC_LIST1} ${SRC_LIST2} ${SRC_LIST3})

target_link_libraries(${PROJECT_NAME}  ${CMAKE_THREAD_LIBS_INIT})
```

配置好`CMakeLists.txt`之后，就可以尝试编译项目，首先创建`build`文件夹，防止文件和项目混在一起。
```bash
mkdir build
cd build
```

然后使用`cmake`生成`Makefile`:
```bash
cmake ..
```
然后使用make指令即可生成对应的可执行文件
```bash
make
```

生成的可知性文件被放在`build/test`中，只需要使用`./test/WebServer`即可运行服务器

这是一个非常简单的`CMakeLists.txt`，只是保证了当前项目可以运行起来，如果需要更加复杂的变化，就需要学习相关的代码，并查阅相应的资料了
