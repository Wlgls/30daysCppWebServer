
#include "HttpRequest.h"

#include <string>
#include <map>
#include <iostream>

HttpRequest::HttpRequest() : method_(kInvalid), version_(kUnknown){
};

HttpRequest::~HttpRequest(){};

void HttpRequest::SetVersion(const std::string & ver){
    if(ver == "1.0"){
        version_ = Version::kHttp10;
    }else if(ver == "1.1"){
        version_ = Version::kHttp11;
    }else{
        version_ = Version::kUnknown;
    }
}

HttpRequest::Version HttpRequest::version() const{
    return version_;
}

std::string HttpRequest::GetVersionString() const{
    std::string ver;
    if (version_ == Version::kHttp10)
    {
        ver = "http1.0";
    }
    else if (version_ == Version::kHttp11){
        ver = "http1.1";
    }else{
        ver = "unknown";
    }
    return ver;
}

bool HttpRequest::SetMethod(const std::string &_method){
    if(_method == "GET"){
        method_ = Method::kGet;
    }else if(_method == "POST"){
        method_ =  Method::kPost;
    }else if(_method == "HEAD"){
        method_ = Method::kHead;
    }else if(_method == "PUT"){
        method_ = Method::kPut;
    }else if(_method == "Delete"){
        method_ = Method::kDelete;
    }else{
        method_ = Method::kInvalid;
    }
    return  method_ != Method::kInvalid;
}

HttpRequest::Method HttpRequest::method() const{
    return method_;
}
std::string HttpRequest::GetMethodString() const{
    std::string _method;
    if (method_ == Method::kGet)
    {
        _method = "GET";
    }
    else if (method_ == Method::kPost)
    {
        _method = "POST";
    }
    else if (method_ == Method::kHead){
        _method =  "HEAD";
    }
    else if(method_ == Method::kPut){
        _method = "PUT";
    }
    else if (method_ == Method::kDelete)
    {
        _method =  "DELETE";
    }else{
        _method = "INVALID";
    }
    return _method;
}

void HttpRequest::SetUrl(const std::string &url){
    url_ = url;
}
const std::string & HttpRequest::url() const{
    return url_;
}

void HttpRequest::SetRequestParams(const std::string &key, const std::string &value){
    request_params_.insert(std::make_pair(key, value));
}
std::string HttpRequest::GetRequestValue(const std::string &key) const{
    std::string ret;
    auto it = headers_.find(key);
    return it == headers_.end() ? ret : it->second;
}
const std::map<std::string, std::string> & HttpRequest::request_params() const{
    return request_params_;
}

void HttpRequest::SetProtocol(const std::string &str){
    protocol_ = str;
}
const std::string & HttpRequest::protocol() const{
    return protocol_;
}

void HttpRequest::AddHeader(const std::string &field, const std::string &value){
    headers_.insert(std::make_pair(field, value));
}
std::string HttpRequest::GetHeader(const std::string &field) const{
    std::string result;
    auto it = headers_.find(field);
    if(it!=headers_.end()){
        result = it->second;
    }
    return result;
}
const std::map<std::string, std::string> & HttpRequest::headers() const{
    return headers_;
}


void HttpRequest::SetBody(const std::string &str){
    body_ = str;
}
const std::string & HttpRequest::body() const{
    return body_;
}
