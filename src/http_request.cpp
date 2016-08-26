#include <http_request.hpp>
#include <iostream>

#include <string_utils.hpp>

namespace
{
void strip_cr(std::string& str)
{
    if (!str.empty())
        if (str.back() == '\r')
            str.pop_back();
}
void strip_cr(std::vector<std::string>& strings)
{
    if (!strings.empty())
        for (auto& str : strings)
            strip_cr(str);
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
        auto lines = util::string::split(buffer_, '\n');
        if (lines.size() > 1)
        {
            strip_cr(lines);
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
        auto pos = buffer_.find_last_of('\n');
        if (pos == std::string::npos)
            return;
        auto to_process = buffer_.substr(0, pos);

        if (buffer_.size() > pos)
            buffer_ = buffer_.substr(pos);

        auto lines = split(to_process, '\n');
        strip_cr(lines);
        for (const auto& line : lines)
        {
            // if (line.empty())
            // parse_state_ = ParseState::DONE;
            // else
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
