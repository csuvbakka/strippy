#include <http_response.hpp>
#include <string_utils.hpp>

#include <iostream>

namespace http
{
Response::Response(const std::string& data)
    : data_(data)
{
    if (data.empty())
        return;

    std::string headers;
    std::tie(headers, content_) = util::string::split_first(data, "\r\n\r\n");

    // std::cout << "headers" << std::endl;
    // std::cout << headers << std::endl;
    // std::cout << "content" << std::endl;
    // std::cout << content_ << std::endl;

    auto lines = util::string::split(headers, '\n');
    lines.erase(lines.begin());

    std::string header, header_data;
    for (const auto& line : lines)
    {
        std::tie(header, header_data) = util::string::split_first(line, ':');
        // std::cout << header << ":" << header_data << std::endl;
        if (!header.empty())
            headers_[header] = util::string::trim(header_data);
    }
}
}
