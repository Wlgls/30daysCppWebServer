# day27-处理静态文件，实现POST请求

在之前的http服务器中，服务器对客户端的返回直接定义在了`c++`文件中，但是在实际应用中，一般都需要编写相应的前端页面，然后直接返回页面。

在`c++`中，实现这种方法并不复杂，只需要将相应的文件读取保存为字符串，之后依照正常流程进行发送即可。

我们使用`fstream`标准库来处理读取文件，其他的方法也可以达到相同的目的，此处就不再赘述。

整体的读取文件的代码十分简单。只需要打开文件并读取即可。我们在`http_server.cpp`中定义了这个文件。

```c++

std::string ReadFile(const std::string& path){
    std::ifstream is(path.c_str(), std::ifstream::in);

    // 寻找文件末端
    is.seekg(0, is.end);

    // 获取长度
    int flength = is.tellg();

    //重新定位
    is.seekg(0, is.beg);
    char * buffer = new char[flength];

    // 读取文件
    is.read(buffer, flength);
    std::string msg(buffer, flength);
    return msg;
}
```

随后，我们在设置回应时，先读取文件，然后在将作为回应体即可，需要注意的是，根据不同的数据类型，需要设定不同的`ContentType`
```c++
void HttpResponseCallback(const HttpRequest &request, HttpResponse *response)
{
    if(request.method() != HttpRequest::Method::kGet){
        response->SetStatusCode(HttpResponse::HttpStatusCode::k400BadRequest);
        response->SetStatusMessage("Bad Request");
        response->SetCloseConnection(true);
    }

    {
        std::string url = request.url();
        if(url == "/"){
            std::string body = ReadFile("../static/index.html");
            response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
            response->SetBody(body);
            response->SetContentType("text/html");
        }else if(url == "/mhw"){
            std::string body = ReadFile("../static/mhw.html");
            response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
            response->SetBody(body);
            response->SetContentType("text/html");
        }else if(url == "/cat.jpg"){
            std::string body = ReadFile("../static/cat.jpg");
            response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
            response->SetBody(body);
            response->SetContentType("image/jpeg");
        }else{
            response->SetStatusCode(HttpResponse::HttpStatusCode::k404NotFound);
            response->SetStatusMessage("Not Found");
            response->SetBody("Sorry Not Found\n");
            response->SetCloseConnection(true);
        }
    }
    return;
}
```

上述实现了简单的`get`请求，但是针对登陆注册等，使用`get`请求会将帐号密码暴露在`url`中，因此一般使用`post`请求。`post`和`get`请求的区别具体可以在网络搜索。在本教程中将简单的简述一下如何处理`post`请求。

在`index.html`中，有一个简单的登陆注册界面，通过点击登陆键将提交一个`Content-Type`为`application/x-www-form-urlencoded`的`post`请求，这种格式将提交一个类似`URL`中的`key=value&...`形式。此外还有`application/json`等此处并不再赘述，具体可以参见`HTTP`的各个头部字段。

因此，当我们接收到一个`post`请求时，依然需要首先判定它所请求的路径，针对不同的路径会进行不同的操作，由于本次只有`/login`的路径，因此只对其进行了设置，之后，开始分析请求体，对于一个登陆注册而言，其请求体是`username=username&password=password`这种形式的，因此对其解析即可。

```c++
if( request.method() == HttpRequest::Method::kPost){
    if(url == "/login"){
        // 进入登陆界面
        std::string rqbody = request.body();

        // 解析
        int usernamePos = rqbody.find("username=");
        int passwordPos = rqbody.find("password=");

        usernamePos += 9; // "username="的长度
        passwordPos += 9; // 

        // 找到中间分割符
        size_t usernameEndPos = rqbody.find('&', usernamePos);
        size_t passwordEndPos = rqbody.length();

        // Extract the username and password substrings
        std::string username = rqbody.substr(usernamePos, usernameEndPos - usernamePos);
        std::string password = rqbody.substr(passwordPos, passwordEndPos - passwordPos);

        // 简单的测试
        if (username == "wlgls"){
            response->SetBody("login ok!\n");
        }
        else{
            response->SetBody("error!\n");
        }
        response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
        response->SetStatusMessage("OK");
        response->SetContentType("text/plain");
    }
}
```

上述程序只是简单的应用，在现实中，还需要更加复杂的逻辑关系和代码，但是其整体的框架与这种形式是十分相似的，只需要进行内容的扩展即可。

