#pragma once

#include <string>
#include <unordered_map>

namespace http
{
enum class RequestType
{
    GET,
    POST,
    OTHER
};

class Request
{
public:
    Request(const std::string& data);

    std::string data() { return data_; }
    std::string request_line() { return request_line_; }
    std::string operator[](const std::string& header);

private:
    std::string data_;
    RequestType request_type_;
    std::unordered_map<std::string, std::string> headers_;
    std::string request_line_;
};
}
