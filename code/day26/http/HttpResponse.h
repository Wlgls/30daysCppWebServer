#pragma once

#include<string>
#include<utility>
#include <map>



class HttpResponse{
    public:
        enum HttpStatusCode
        {
            kUnkonwn = 1,
            k100Continue = 100,
            k200K = 200,
            k400BadRequest = 400,
            k403Forbidden = 403,
            k404NotFound = 404,
            k500internalServerError = 500
        };
        HttpResponse(bool close_connection);
        ~HttpResponse();

        void SetStatusCode(HttpStatusCode status_code); // 设置回应码
        void SetStatusMessage(const std::string &status_message);
        void SetCloseConnection(bool close_connection);

        void SetContentType(const std::string &content_type); 
        void AddHeader(const std::string &key, const std::string &value); // 设置回应头

        void SetBody(const std::string &body);
        std::string message(); // 将信息加入到buffer中。

        bool IsCloseConnection();

    private:
        // static const std::string server_name_;

        std::map<std::string, std::string> headers_;

        HttpStatusCode status_code_;
        std::string status_message_;
        std::string body_;
        bool close_connection_;
};