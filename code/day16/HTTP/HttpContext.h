/*
对request的解析

在当前版本中，对http的解析将仅仅使用简单的状态转换机，并且输入是一个string字符串，而不是一个buffer

主从装换机

从状态机读取报文的一行，然后，主状态机负责对该行进行解析



*/
#pragma once
#include <string>
#include <memory>
class HttpRequest;


#define CR '\r'
#define LF '\n'

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

    BEFORE_URL_PARAM_KEY, // URL请求参数键之前
    URL_PARAM_KEY, // URL请求参数键
    BEFORE_URL_PARAM_VALUE, // URL请求参数值之前
    URL_PARAM_VALUE, // URL请求参数值

    BEFORE_PROTOCOL, // 协议解析之前
    PROTOCOL,        // 协议

    BEFORE_VERSION, // 版本开始前
    VERSION_SPLIT,  // 版本分割符
    VERSION,        // 版本

    HEADER,
    HEADER_KEY, //

    HEADER_BEFORE_COLON, // 请求头冒号之前
    HEADER_AFTER_COLON,  // 请求头冒号
    HEADER_VALUE,        // 请求值

    WHEN_CR, // 遇到一个回车

    CR_LF, // 回车换行

    CR_LF_CR, // 回车换行之后的状态

    BODY, // 请求体

    COMPLETE, // 完成

};

class HttpContext
{

public:

    // 状态转换机，保存解析的状态

    HttpContext();
    ~HttpContext();

    bool ParaseRequest(const char *begin, int size);
    bool GetCompleteRequest();
    HttpRequest *request();
    void ResetContextStatus();

private:
    std::unique_ptr<HttpRequest> request_;
    HttpRequestParaseState state_;
};
