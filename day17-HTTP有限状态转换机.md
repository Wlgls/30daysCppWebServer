# day17-HTTP有限状态转换机

为了实现`Http`服务器，首先需要对`Http`进行解析。对于`Http`请求报文。首先由四个部分组成，分别是请求行，请求头，空行和请求体组成。

其格式为
```
请求方法 URL HTTP/版本号
请求头字段
空行
body
```
例如
```
GET /HEELO HTTP/1.1\r\n
Host: 127.0.0.1:1234\r\n
Connection: Keep-alive\r\n
Content-Length: 12\r\n
\r\n
hello world;
```
可以看出，其格式是非常适合使用有限状态转换机。

为了实现这个功能，首先需要创建一个`HttpContext`解析器。这个解析器需要一个`HttpRequest`类来保存解析结果。
对于`HttpRequest`，他主要保存请求中的各种信息，如METHOD，URL等
```c++
class HttpRequest{
private:
    Method method_; // 请求方法
    Version version_; // 版本

    std::map<std::string, std::string> request_params_; // 请求参数

    std::string url_; // 请求路径

    std::string protocol_; // 协议

    std::map<std::string, std::string> headers_; // 请求头

    std::string body_; // 请求体
};
```

解析时，我们逐个字符遍历客户端发送的信息，首先设定`HttpContext`的初始状态为`START`。当我们遇到大写字母时，必然会是请求方法，因此此时，我们将此时的状态转成`METHOD`。当继续遇到大写字母时，说明`METHOD`的解析还在进行，而一旦遇到空格，说明我们的请求方法解析就结束了，我们使用`start`和`end`两个指针指向`METHOD`的起始位置和结束位置，获取相应的结果送入到`HttpRequest`中，之后更新`start`和`end`的位置，并更新当前的解析状态，进行下一个位置的解析。

```c++
class HttpContext
{
public:
    enum HttpRequestParaseState
    {
        kINVALID,         // 无效
        kINVALID_METHOD,  // 无效请求方法
        kINVALID_URL,     // 无效请求路径
        kINVALID_VERSION, // 无效的协议版本号
        kINVALID_HEADER,  // 无效请求头

        START,  // 解析开始
        METHOD, // 请求方法

        BEFORE_URL, // 请求连接前的状态，需要'/'开头
        IN_URL,     // url处理
        ...
        COMPLETE, // 完成
    };
    // 状态转换机，保存解析的状态
    bool ParaseRequest(const char *begin, int size);

private:
    std::unique_ptr<HttpRequest> request_;
    HttpRequestParaseState state_;
};

// HttpContext.cpp
bool HttpContext::ParamRequest(const char *begin, int size){
    //
    char *start = const_cast<char *>(begin);
    char *end = start;
    while(state_ != HttpRequestParaseState::kINVALID 
        && state_ != HttpRequestParaseState::COMPLETE
        && end - begin <= size){
        
        char ch = *end; // 当前字符
        switch(state_){
            case HttpRequestParaseState::START:{
                if(isupper(ch)){
                    state_ = HttpRequestParaseState::METHOD;
                }
                break;
            }
            case HttpRequestParaseState::METHOD:{
                if(isblank(ch)){
                    // 遇到空格表明，METHOD方法解析结束，当前处于即将解析URL，start进入下一个位置
                    request_->SetMethod(std::string(start, end));
                    state_ = HttpRequestParaseState::BEFORE_URL;
                    start = end + 1; // 更新下一个指标的位置
                    }
                break;
            }
            case HttpRequestParaseState::BEFORE_URL:{
                // 对请求连接前的处理，请求连接以'/'开头
                if(ch == '/'){
                    // 遇到/ 说明遇到了URL，开始解析
                    state_ = HttpRequestParaseState::IN_URL;
                }
                break;
            }
            ...
        }
        end ++;
    }
}
```

实际上这个版本并不鲁棒，而且性能并不是十分优异，但是作为一个玩具而言，并且去熟悉HTTP协议而言，还是有些价值的。

在本日额外实现了一个`test_httpcontext.cpp`文件用于测试`HttpContext`的效果。在`CMakeLists.txt`加入`http`的路径，并创建`build`文件，在`build`中运行`cmake ..`，之后`make test_context`并运行`./test/test_contxt`即可。

特别的，感谢[若_思CSDN C++使用有限状态自动机变成解析HTTP协议](https://blog.csdn.net/qq_39519014/article/details/112317112)本文主要参考了该博客进行了修改和实现

