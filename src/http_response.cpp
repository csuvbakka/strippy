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
    std::tie(headers, content_) = str::split_first(data, "\r\n\r\n");

    // std::cout << "headers" << std::endl;
    // std::cout << headers << std::endl;
    // std::cout << "content" << std::endl;
    // std::cout << content_ << std::endl;

    auto lines = str::split(headers, '\n');
    lines.erase(lines.begin());

    std::string header, header_data;
    for (const auto& line : lines)
    {
        std::tie(header, header_data) = str::split_first(line, ':');
        // std::cout << header << ":" << header_data << std::endl;
        if (!header.empty())
            headers_[header] = str::trim(header_data);
    }
}
}
