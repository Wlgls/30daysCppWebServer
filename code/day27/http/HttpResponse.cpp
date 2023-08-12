#include "HttpResponse.h"
#include <string>

HttpResponse::HttpResponse(bool close_connection) : 
    status_code_(HttpStatusCode::kUnkonwn), close_connection_(close_connection) , body_type_(HttpBodyType::HTML_TYPE){};

HttpResponse::~HttpResponse(){};

void HttpResponse::SetStatusCode(HttpStatusCode status_code){
    status_code_ = status_code;
}

void HttpResponse::SetStatusMessage(const std::string& status_message){
    status_message_ = std::move(status_message);
}

void HttpResponse::SetCloseConnection(bool close_connection){
    close_connection_ = close_connection;
}

void HttpResponse::SetContentType(const std::string &content_type){
    AddHeader("Content-Type", content_type);
}

void HttpResponse::SetContentLength(const int &len){
    content_length_ = len;
}

int HttpResponse::GetContentLength() { return content_length_; }

void HttpResponse::AddHeader(const std::string &key, const std::string &value){
    headers_[key] = value;
}

void HttpResponse::SetBody(const std::string &body){
    body_ = std::move(body);
}

bool HttpResponse::IsCloseConnection(){
    return close_connection_;
}

std::string HttpResponse::message(){
    return beforebody() + body_;
}

std::string HttpResponse::beforebody(){
    std::string message;
    message += ("HTTP/1.1 " +
                std::to_string(status_code_) + " " +
                status_message_ + "\r\n"
    );
    if(close_connection_){
        message += ("Connection: close\r\n");
    }else{
        message += ("Connection: Keep-Alive\r\n");
    }
    message += ("Content-Length: " + std::to_string(content_length_) + "\r\n");

    for (const auto&header : headers_){
        message += (header.first + ": " + header.second + "\r\n");
    }

    message += "Cache-Control: no-store, no-cache, must-revalidate\r\n";
    message += "\r\n";

    return message;
}

int HttpResponse::filefd() const { return filefd_; };
HttpResponse::HttpBodyType HttpResponse::bodytype() const { return body_type_; };
void HttpResponse::SetFileFd(int filefd) { filefd_ = filefd; };
void HttpResponse::SetBodyType(HttpBodyType bodytype) { body_type_ = bodytype; };
