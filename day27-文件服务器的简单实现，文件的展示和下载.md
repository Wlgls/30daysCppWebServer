# day27-文件服务器的简单实现，文件的展示和下载

对于一个文件服务器而言，它应该包含两个最基本的功能，即 1）文件列表的展示，2）文件的上传和下载和删除

对于文件列表的展示，一般情况下，存放的文件并不是一个固定的数量，因此在展示时，就需要动态的去获得文件列表，这个功能就需要在后端实现(我对前端不太懂，ChatGPT告诉我前端不能动态的获取文件列表)。

那么在后端需要实现的就是获取当前文件夹的文件列表，并生成对应的前端界面，然后才将所有的信息发送到客户端。

首先，我们需要实现对目录的遍历。这个操作是非常简单的，利用`opendir`和`readdir`两个函数就可以非常简单的遍历指定目录的所有文件，我们将文件名称存在`filelist`中，方便下一步处理
```c++
void FindAllFiles(const std::string& path, std::vector<std::string> &filelist){
    DIR *dir;
    struct dirent *dir_entry = NULL;
    if((dir = opendir(path.c_str())) == NULL){
        LOG_ERROR << "Opendir " << path << " failed";
        return;
    }
    
    while((dir_entry = readdir(dir))!= NULL){
        std::string filename = dir_entry->d_name;
        if (filename != "." && filename != ".."){
            filelist.push_back(filename);
        }
            
    }
}
```

当我们获得相应的的文件名称后，可以很方便的将每一个文件名称都生成一个前端模板。例如这样的一个模板，对于一个文件，它展示了文件名称，并在之后提供两个按键，点击这两个按键将发出`GET`请求。

```html
<tr> 示例
    <td>a.txt</td>
    <td>
        <a href="/download/a.txt">下载</a>
        <a href="/download/a.txt">删除</a>
    </td>
</tr>
```

如果想要将其中的`a.txt`替换成我们的文件，我们只需要进行简单的字符串处理即可。

例如，
```c++
std::string file = "";
for (auto filename : filelist)
{
    //将fileitem中的所有filename替换成
    file += "<tr><td>" + filename + "</td>" +
            "<td>" +
            "<a href=\"/download/" + filename + "\">下载</a>" +
            "<a href=\"/delete/" + filename + "\">删除</a>" +
            "</td></tr>" + "\n";
}
```

这样就可以为每一个文件都生成一个前端的展示，而将这些展示加入到相应的`html`页面也可以通过非常简单的操作实现，例如，我们在`html`文件中相应的位置嵌入`<!--filelist-->`这样的代码。这样我们就可以直接读取`html`文件，并找到相应的位置直接替换即可。

```c++
// 构建filelist.html
std::string BuildFileHtml(){
    std::vector<std::string> filelist;
    // 以/files文件夹为例
    FindAllFiles("../files", filelist);


    // 为文件生成模板
    std::string file = "";
    for (auto filename : filelist)
    {
        //将fileitem中的所有filename替换成
        file += "<tr><td>" + filename + "</td>" +
                "<td>" +
                "<a href=\"/download/" + filename + "\">下载</a>" +
                "<a href=\"/delete/" + filename + "\">删除</a>" +
                "</td></tr>" + "\n";
    }


    //生成html页面
    // 主要通过将<!--filelist-->直接进行替换实现
    std::string tmp = "<!--filelist-->";
    std::string filehtml = ReadFile("../static/fileserver.html");
    filehtml = filehtml.replace(filehtml.find(tmp), tmp.size(), file);
    return filehtml;
}
```

通过如上操作，就实现了对简单的文件展示页面。当客户端请求相应的资源时，只需要调用上述函数，并将生成的字符串作为我们的响应体即可。

在上述的前端页面中，下载和删除都是由`GET`请求实现的，并在`url`中加入了文件名称，因此对于上传和下载，只需要对应的处理即可。由于下载比较繁琐，首先先实现删除操作。

对于删除操作是十分简单的，我们只需要判断当前的请求是否是删除的请求，。并在`url`中提取出要删除的文件的名称，并对其进行删除即可。在删除之后，发送一个重定向报文，将页面重新指向文件列表即可。
```c++ 
if(url.substr(0, 7) == "/delete") {
            // 删除特定文件，由于使用get请求，并且会将相应删掉文件的名称放在url中
    RemoveFile(url.substr(8));
    // 发送重定向报文，删除后返回自身应在的位置
    response->SetStatusCode(HttpResponse::HttpStatusCode::k302K);
    response->SetStatusMessage("Moved Temporarily");
    response->SetContentType("text/html");
    response->AddHeader("Location", "/fileserver");
}
```

对于文件下载可以通过将文件内容进行读取加入到`response`的`body`中，然后传输给客户端即可。但是这种操作需要数据在内核空间和内存空间来回复制，从而会严重影响高并发的性能。面对这种场景就可以使用零拷贝技术，从而减少用户态和内核态的上下文交互。具体的零拷贝的概念我参考了[这篇博客](https://juejin.cn/post/6995519558475841550)。

在本文中，采用了`sendfile`函数来实现零拷贝，由于`sendfile`只能传输文件，那么在实际应用中，就需要将响应报文的消息体单独进行发送。并且在请求头发送结束后，对请求体进行发送。

我们首先在`TcpConnection`中定义`SendFile`操作。这个操作暂时非常简单，一直发送直到文件发送完成(因此面对大文件可能会造成阻塞)。
```c++
void TcpConnection::SendFile(int filefd, int size){
    ssize_t send_size = 0;
    ssize_t data_size = static_cast<ssize_t>(size);
    // 一次性把文件写完，虽然肯定不行。
    while(send_size < data_size){

        ssize_t bytes_write = sendfile(connfd_, filefd, (off_t *)&send_size, data_size - send_size);

        if (bytes_write == -1)
        {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)){
                continue;
            }else{
                //continue;
                break;
            }
        }
        send_size += bytes_write;
    }
}
```

当我们接收到`download`时，将设定`response`的相关参数，其中，主要需要指定文件的描述符和相应的文件大小。为了区分正常响应和文件响应，在`response`中添加一个成员变量`body_type_`用于指示当前响应的类别。
```c++
void HttpResponseCallback(const HttpRequest &request, HttpResponse *response){
    if(url.substr(0, 9) == "/download"){
        DownloadFile(url.substr(10), response);
    }
}


void DownloadFile(const std::string &filename, HttpResponse *response){
    int filefd = ::open(("../files/" + filename).c_str(), O_RDONLY);
    if(filefd == -1){
        LOG_ERROR << "OPEN FILE ERROR";
        // 文件打开失败，重定向当前页面
        response->SetStatusCode(HttpResponse::HttpStatusCode::k302K);
        response->SetStatusMessage("Moved Temporarily");
        response->SetContentType("text/html");
        response->AddHeader("Location", "/fileserver");
    }else{
        // 获取文件信息
        struct stat fileStat;
        fstat(filefd, &fileStat);
        // 设置响应头字段
        response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
        response->SetContentLength(fileStat.st_size);
        response->SetContentType("application/octet-stream");
        
        response->SetBodyType(HttpResponse::HttpBodyType::FILE_TYPE);
        // 设置文件
        response->SetFileFd(filefd);
    }
}
```

通过上述设定就可以设定`response`的响应类型，并在服务器对响应处理时进行响应的操作，例如如果`body_type_ == FILE_TYPE`就执行发送文件的操作，在这个操作中，将先发送响应报文头部字段，随后在发送文件。
```c++
void HttpServer::onRequest(const TcpConnectionPtr &conn, const HttpRequest &request){
    if(response.bodytype() == HttpResponse::HttpBodyType::HTML_TYPE){
        conn->Send(response.message());
    }else{
        // 考虑到头部字段数据量不多，直接发送完头部字段后，直接发送文件。
        conn->Send(response.beforebody());
        //sleep(1);
        conn->SendFile(response.filefd(), response.GetContentLength());

        // 发送之后关闭文件
        int ret = ::close(response.filefd());
        if(ret == -1){
            LOG_ERROR << "Close File Error";
        }else{
            LOG_INFO << "Close File Ok";
        }
        void(ret);
    }
}

```

以上就实现了一个简单的文件展示和文件下载/删除的服务器。当请求展示页面时，将返回一个HTML界面，之后根据用户不同的操作进行不同的回调，对于删除操作非常简单，只需要收到请求时，删除对应的文件之后重定向当前页面即可。对于下载的操作可能稍微复杂一些，为了保证高性能的实现，采用了`sendfile`函数，这也导致了在发送文件时，需要先发送响应头，之后再发送响应体，为了实现这个功能，在`HttpResponse`类中增加了新的成员变量，并根据不同响应报文进行不同的操作。

这个服务器还存在许多问题，例如当处理大文件时，一次性发完所有数据时会发生严重的堵塞。另外，先发送响应头然后直接发送响应体的操作在极端情况下，如果一次没有把响应头完全发送，存在一部分数据等待后续发送，不知道之后直接调用`sendfile`是否会导致错误。因此在本教程中仅仅实现了简单的应用，对于实际场景更加复杂的内容并没有考虑到。

此外，随着代码量的增加，`http_server.cpp`的代码变得更加复杂，因此需要进行一定程度的细化和重构，但是并不在本日进行了。
