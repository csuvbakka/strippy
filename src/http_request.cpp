#include <http_request.hpp>
#include <iostream>

#include <string_utils.hpp>

namespace http
{
Request::Request(const std::string& data)
    : data_(data)
{
    if (data.empty())
        return;

    auto lines = util::string::split(data, '\n');

    request_line_ = lines[0];
    if (util::string::starts_with(request_line_, "GET"))
        request_type_ = RequestType::GET;
    else if (util::string::starts_with(request_line_, "POST"))
        request_type_ = RequestType::POST;
    else
        request_type_ = RequestType::OTHER;

    lines.erase(lines.begin());

    std::string header, header_data;
    for (const auto& line : lines)
    {
        std::tie(header, header_data) = util::string::split_first(line, ':');
        std::cout << header << ":" << header_data << std::endl;
        if (!header.empty())
            headers_[header] = util::string::trim(header_data);
    }
}

std::string Request::operator[](const std::string& header_string)
{
    auto it = headers_.find(header_string);
    if (it != headers_.end())
        return it->second;
    else
        return {};
}
}
