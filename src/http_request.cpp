#include <http_request.hpp>
#include <iostream>

#include <string_utils.hpp>

namespace
{
void strip_cr(mystr::MyString& str)
{
    if (!str.is_empty())
        if (str.back() == '\r')
            str.pop_back();
}
}

namespace http
{
Request::Request(mystr::MyStringBuffer& buffer)
    : parse_state_(ParseState::NOT_STARTED)
    , buffer_(buffer)
    , request_line_(buffer_)
{
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
    {
        while (buffer_.starts_with("\r\n"))
            buffer_.erase_head(2); // RFC2616 - 4.1
        parse_state_ = ParseState::PARSING_REQUEST_LINE;
    }

    mystr::MyString header(buffer_);
    do
    {
        auto first_nl_pos = buffer_.find_first_of('\n');
        if (first_nl_pos == buffer_.end())
            return;

        auto line = mystr::MyString{buffer_, buffer_.begin(), first_nl_pos};
        strip_cr(line);
        buffer_.erase_head_until(std::next(first_nl_pos));

        if (parse_state_ == ParseState::PARSING_REQUEST_LINE)
        {
            request_line_ = line;
            parse_state_ = ParseState::PARSING_HEADERS;
        }
        else if (parse_state_ == ParseState::PARSING_HEADERS)
        {
            using namespace util::string;

            if (line.is_empty())
                parse_state_ = ParseState::DONE;
            else if (line.starts_with(' ') || line.starts_with('\t'))
                add_line_to_multiline_header(line, header);
            else
            {
                mystr::MyString header_data(buffer_);
                std::tie(header, header_data) = line.split_first(':');
                if (!header.is_empty() && !header_data.is_empty())
                    headers_[header.str()] = trim_copy(header_data).str();
            }
        }
    } while (!buffer_.is_empty());
}

void Request::add_line_to_multiline_header(const mystr::MyString& line,
                                           const mystr::MyString& header)
{
    auto non_whitespace_pos = line.find_first_not_of("\t ");
    if (non_whitespace_pos != line.end())
        if (!header.is_empty())
            headers_[header.str()] += line.substr(non_whitespace_pos).str();
}
}
