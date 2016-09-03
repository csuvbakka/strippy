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
    , content_(buffer_)
    , content_length_(0)
{
}

std::string Request::operator[](const std::string& header_string) const
{
    auto it = headers_.find(header_string);
    if (it != headers_.end())
        return it->second;
    else
        return {};
}

void Request::process_buffer()
{
    if (parse_state_ == ParseState::DONE)
        return;

    if (parse_state_ == ParseState::NOT_STARTED)
    {
        while (buffer_.starts_with("\r\n"))
            buffer_.erase_head(2); // RFC2616 - 4.1
        parse_state_ = ParseState::PARSING_REQUEST_LINE;
    }
    else if (parse_state_ == ParseState::PARSING_CONTENT)
    {
        if (content_.is_empty())
            content_ = string_view(buffer_, buffer_.begin(), buffer_.end());
        else
            content_ = string_view(buffer_, content_.begin(), buffer_.end());

        buffer_.erase_head_until(buffer_.end());
        if (content_.length() > content_length_)
            content_.erase_tail(content_.length() - content_length_);

        if (content_.length() == content_length_)
            parse_state_ = ParseState::DONE;

        return;
    }

    string_view header(buffer_);
    do
    {
        auto first_nl_pos = buffer_.find_first_of('\n');
        if (first_nl_pos == buffer_.end())
            return;

        auto line = string_view{buffer_, buffer_.begin(), first_nl_pos};
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
            {
                static const std::string content_length_str = "Content-Length";
                std::string l = headers_[content_length_str];
                if (l.empty())
                    parse_state_ = ParseState::DONE;
                else
                {
                    parse_state_ = ParseState::PARSING_CONTENT;
                    content_length_ = std::stoi(l);
                }
            }
            else if (line.starts_with(' ') || line.starts_with('\t'))
                add_line_to_multiline_header(line, header);
            else
            {
                string_view header_data(buffer_);
                std::tie(header, header_data) = line.split_first(':');
                if (!header.is_empty() && !header_data.is_empty())
                    headers_[header.str()] = trim_copy(header_data).str();
            }
        }
    } while (!buffer_.is_empty());
}

void Request::add_line_to_multiline_header(const string_view& line,
                                           const string_view& header)
{
    auto non_whitespace_pos = line.find_first_not_of("\t ");
    if (non_whitespace_pos != line.end())
        if (!header.is_empty())
            headers_[header.str()] += line.substr(non_whitespace_pos).str();
}
}
