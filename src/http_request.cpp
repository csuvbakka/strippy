#include <http_request.hpp>
#include <iostream>

#include <string_utils.hpp>

namespace
{
std::string strip_cr(const std::string& str)
{
    if (!str.empty())
        if (str[str.size() - 1] == '\r')
            return str.substr(0, str.size() - 1);

    return str;
}
std::vector<std::string> strip_cr(const std::vector<std::string>& strings)
{
    if (!strings.empty())
    {
        std::vector<std::string> output;
        output.reserve(strings.size());

        for (const auto& str : strings)
            if (str[str.size() - 1] == '\r')
                output.push_back(str.substr(0, str.size() - 1));
            else
                output.push_back(str);

        return output;
    }

    return strings;
}
}

namespace http
{
Request::Request()
    : parse_state_(ParseState::NOT_STARTED)
{
}

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

void Request::process_buffer()
{
    if (parse_state_ == ParseState::NOT_STARTED)
        parse_state_ = ParseState::PARSING_REQUEST_LINE;

    if (parse_state_ == ParseState::PARSING_REQUEST_LINE)
    {
        auto lines = strip_cr(util::string::split(buffer_, '\n'));
        if (lines.size() > 1)
        {
            request_line_ = lines[0];
            lines.erase(lines.begin());
            buffer_ = util::string::join(lines, "\r\n");
            parse_state_ = ParseState::PARSING_HEADERS;
        }
    }

    if (parse_state_ == ParseState::PARSING_HEADERS)
    {
        using namespace util::string;
        std::string header, header_data;
        auto lines = strip_cr(split(buffer_, '\n'));
        for (const auto& line : lines)
        {
            if (line.empty())
                parse_state_ = ParseState::DONE;
            else
            {
                std::tie(header, header_data) = split_first(line, ':');
                if (!header.empty() && !header_data.empty())
                {
                    headers_[trim(header)] = trim(header_data);
                }
            }
        }
    }
}

Request& operator>>(Request& lhs, const std::string& rhs)
{
    lhs.buffer_ += rhs;
    lhs.process_buffer();
    return lhs;
}
}
