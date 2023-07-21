# day29-文件的上传

在之前的工作中，实现了文件的展示和下载功能，而在文件的上传与上述两种具有很大的区别。

在文件的上传时，我们在前端简单设置了一个接口，规定上传时的方法为`POST`方法，并且设定`content-type`为`multipart/form-data`。至于为什么这么设定，可以查看`HTML`的`Content-Type`不同设置的不同之处。
```html
<form action="/upload" method="post" enctype="multipart/form-data">
    <input type="file" name="file" id="fileInput" accept=".txt,.pdf,.doc,.docx,.jpg,.png">
    <button type="submit">上传文件</button>
</form>
```

通过上述表单，当用户点击上传文件时，浏览器并不会只想服务器发送一个请求报文，而是将报文分为两个部分分别发送。
```shell
// 第一次信息
POST /upload HTTP/1.1
Host: 127.0.0.1:1252
Connection: keep-alive
Content-Length: 180
Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryVarr6kdO6hEAmEvh



// 第二次信息
------WebKitFormBoundaryVarr6kdO6hEAmEvh
Content-Disposition: form-data; name="file"; filename="d.txt"
Content-Type: text/plain

d
------WebKitFormBoundaryVarr6kdO6hEAmEvh--
```

可以看到，在第一次发送时，会首先将请求报文的请求头字段发送过来，之后会再发送文件信息，主要包括文件名称和文件内容。这样之前接收请求的代码逻辑就不够完善了。之前的代码在接收到请求时，会直接对其进行解析，如果解析成功就对请求进行处理。
```c++
HttpContext *context = conn->context();
if (!context->ParaseRequest(conn->read_buf()->RetrieveAllAsString()))
{

    LOG_INFO << "HttpServer::onMessage : Receive non HTTP message";
    conn->Send("HTTP/1.1 400 Bad Request\r\n\r\n");
    conn->HandleClose();
}

if (context->GetCompleteRequest())
{

    onRequest(conn, *context->request());
    context->ResetContextStatus();
}
```

之前对于一个HTTP请求使用了上述逻辑，但是上述代码在面对上传文件时会存在严重问题，当上传文件时，对于第一个接收到的报文会认定为HTTP请求，但是遇到第二个时，就会存在问题。导致连接关闭。

对于此，可以通过两种方法，第一个是可以在收到第一个连接时，设置标志符，让服务器认定第二个虽然不是请求报文，但是仍然有用。第二个则是对context解析器进行一定程度的修改，在解析时如果遇到`content-length`但是又没有收到这么长的消息体时，就暂时保持在当前解析状态，等待后续接收数据。从而将一个报文组合在一起。

在此处使用了第二种方法。

在我们进行解析时，当进行到`body`时，只有判定接收到的数据与`content-length`相等，才会认定为解析成功。
```c++
case HttpRequestParaseState::BODY:{      
    int bodylength = size - (end - begin);
    request_->SetBody(std::string(start, start + bodylength));

    if (bodylength >= atoi(request_->GetHeader("Content-Length").c_str()))
    {
        state_ = HttpRequestParaseState::COMPLETE;
    }
    break;
}
```

而且无论是达到了`CONPLETE`还是`BODY`都会认定成功。`return state_ == HttpRequestParaseState::COMPLETE || state_ == HttpRequestParaseState::BODY;`，通过这样的操作，在第一次解析时，我们的`context`的状态保持在`HttpRequestParaseState::BODY`,由于还没有完成，因此也不会对该请求作出响应，当再接收数据时，直接从`BODY`开始解析，如果与之前保存的`Content-Length`一致，说明收到了一个完整的报文，解析完成。此时，才会对请求作出响应。

在响应该请求时，会判断`Content-Type`中是否包含`multipart/form-data`,如果包含，则很大概率代表该请求是一个文件上传请求，此时就可以通过是请求体分析，获取内部存储的文件名和文件信息等相关要素，并创建相应的文件写入数据即可。

```c++
if (request.GetHeader("Content-Type").find("multipart/form-data") != std::string::npos){
    // 对文件进行处理
    //
    // 先找到文件名，一般第一个filename位置应该就是文件名的所在地。
    // 从content-type中找到边界
    size_t boundary_index = request.GetHeader("Content-Type").find("boundary");
    std::string boundary = request.GetHeader("Content-Type").substr(boundary_index + std::string("boundary=").size());

    std::string filemessage = request.body();
    size_t begin_index = filemessage.find("filename");
    if(begin_index == std::string::npos ){
        LOG_ERROR << "cant find filename";
        return;
    }
    begin_index += std::string("filename=\"").size();
    size_t end_index = filemessage.find("\"\r\n", begin_index); // 能用

    std::string filename = filemessage.substr(begin_index, end_index - begin_index);

    // 对文件信息的处理
    begin_index = filemessage.find("\r\n\r\n") + 4; //遇到空行，说明进入了文件体
    end_index = filemessage.find(std::string("--") + boundary + "--"); // 对文件内容边界的搜寻

    std::string filedata = filemessage.substr(begin_index, end_index - begin_index);
    // 写入文件
    std::ofstream ofs("../files/" + filename, std::ios::out | std::ios::app | std::ios::binary);
    ofs.write(filedata.data(), filedata.size());
    ofs.close();
}
```

此外，后续的重定向报文则与删除类似，并不在此处赘述，可以参见代码`http_server.cpp`的代码。

上述就简单的实现了一个文件的上传处理，在实现功能时，对`HttpContext`做了小小的修订，并在处理请求时，进行了额外的判断，用于处理文件上传。

对于文件的上传和下载，通过这两个教程，可以看出大部分都是基于原有的网络库建立的，对于网络库的修改几乎没有，主要是对接收到的信息进行处理和应用。小部分对网络库的修改也只是使原有的代码更加鲁棒。当然随着需求的不断增加，定制网络库是不可避免的，比如我觉得对于文件的上传和下载，如果重新创建一个服务器`Socket`用于专门的处理文件内容的话应该会更好，这在一定程度会减少大文件上传下载对`TcpConnection`的阻塞，不像现在，必须要傻乎乎的等待文件上传和下载结束才能去浏览其他页面。

但是该教程本质上也只是一个入门级的，了解c++11特性和`Socket`基本用法和`muduo`设计理念的一个教程，因此并不进一步的去设计相应的代码了。有兴趣的话，则可以进一步的去优化和设计相应的代码逻辑。



