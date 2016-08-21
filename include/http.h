#pragma once

#include <string>
#include <unordered_map>

enum class HttpRequestType
{
    GET,
    POST,
    OTHER
};

class HttpRequest
{
public:
    HttpRequest(const std::string& data);

    std::string request_line() { return request_line_; }
    std::string operator[](const std::string& header);

private:
    HttpRequestType request_type_;
    std::unordered_map<std::string, std::string> headers_;
    std::string request_line_;
};
