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

    // [Postcondition] buffer_ will start with the first character
    // after the newline character that ends the request line
    if (parse_state_ == ParseState::PARSING_REQUEST_LINE)
    {
        util::string::erase_head(buffer_, '\n');
        util::string::erase_head(buffer_, "\r\n"); // RFC2616 - 4.1

        auto pos = buffer_.find_first_of('\n');
        if (pos != std::string::npos)
        {
            request_line_ = buffer_.substr(0, pos);
            strip_cr(request_line_);
            if (buffer_.length() > pos)
                buffer_ = buffer_.substr(pos + 1);

            parse_state_ = ParseState::PARSING_HEADERS;
        }
    }

    if (parse_state_ == ParseState::PARSING_HEADERS)
    {
        using namespace util::string;

        auto lines = split(get_headers_to_process(), '\n');
        strip_cr(lines);

        std::string header, header_data;
        for (const auto& line : lines)
        {
            if (line.empty())
                parse_state_ = ParseState::DONE;
            else if (starts_with_whitespace(line))
                add_line_to_multiline_header(line, header);
            else
            {
                std::tie(header, header_data) = split_first(line, ':');
                if (!header.empty() && !header_data.empty())
                    headers_[header] = trim(header_data);
            }
        }
    }
}

// [Invariant] buffer_ will start with the first character after
// the line ending newline character if any
std::string Request::get_headers_to_process()
{
    auto pos = buffer_.find_last_of('\n');
    if (pos == std::string::npos)
        return {};

    auto to_process = buffer_.substr(0, pos);

    if (buffer_.length() > pos)
        buffer_ = buffer_.substr(pos + 1);
    else
        buffer_ = {};

    return to_process;
}

void Request::add_line_to_multiline_header(const std::string& line,
                                           const std::string& header)
{
    auto non_whitespace_pos = line.find_first_not_of("\t ");
    if (non_whitespace_pos != std::string::npos)
        if (!header.empty())
            headers_[header] += line.substr(non_whitespace_pos);
}

Request& operator>>(Request& lhs, const std::string& rhs)
{
    lhs.buffer_ += rhs;
    lhs.process_buffer();
    return lhs;
}
}
