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

但是上述代码在面对上传文件时会存在严重问题，当上传文件时，对于第一个接收到的报文会认定为HTTP请求，但是遇到第二个时，就会存在问题。
