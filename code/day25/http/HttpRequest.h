#pragma once
#include <string>
#include <map>


class HttpRequest{
    public:
        enum Method
        {
            kInvalid = 0,
            kGet,
            kPost,
            kHead,
            kPut,
            kDelete
        };
        enum Version
        {
            kUnknown = 0,
            kHttp10,
            kHttp11
        };

        HttpRequest();
        ~HttpRequest();

        void SetVersion(const std::string &ver); // http版本
        Version version() const;
        std::string GetVersionString() const;

        bool SetMethod(const std::string &method); // 设定请求方法
        Method method() const;
        std::string GetMethodString() const ;

        void SetUrl(const std::string &url); // 请求路径
        const std::string &url() const;

        void SetRequestParams(const std::string &key, const std::string &value);
        std::string GetRequestValue(const std::string &key) const;
        const std::map<std::string, std::string> & request_params() const;
        
        void SetProtocol(const std::string &str);
        const std::string & protocol() const;

        void AddHeader(const std::string &field, const std::string &value); // 添加请求体
        std::string GetHeader(const std::string &field) const;
        const std::map<std::string, std::string> & headers() const;

        void SetBody(const std::string &str);
        const std::string & body() const;

    private:
        Method method_; // 请求方法
        Version version_; // 版本

        std::map<std::string, std::string> request_params_; // 请求参数

        std::string url_; // 请求路径

        std::string protocol_;

        std::map<std::string, std::string> headers_; // 请求头

        std::string body_; // 请求体
};
