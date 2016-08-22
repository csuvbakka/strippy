#pragma once

#include <string>
#include <unordered_map>

namespace http
{
class Response
{
public:
    Response(const std::string& data);

    std::string data() { return data_; }
    std::string operator[](const std::string& header);

private:
    std::string data_;
    std::unordered_map<std::string, std::string> headers_;
    std::string content_;
};
}
