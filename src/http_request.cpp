#include <http_request.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <string_utils.hpp>

namespace
{
void strip_cr(str::string_view<str::character_array>& str)
{
    if (!str.empty())
        if (str.back() == '\r')
            str.pop_back();
}

std::string string_to_hex(const std::string& input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();

    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
        output.push_back(' ');
    }
    return output;
}
}

namespace http
{
Request::Request(str::character_array& buffer)
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
    if (parse_state_ == ParseState::DONE || buffer_.empty())
        return;

    if (parse_state_ == ParseState::NOT_STARTED)
    {
        while (buffer_.starts_with("\r\n"))
            buffer_.erase_head(2); // RFC2616 - 4.1
        parse_state_ = ParseState::PARSING_REQUEST_LINE;
    }
    // else if (parse_state_ == ParseState::PARSING_CONTENT)
    // {
    // if (content_.empty())
    // content_ = string_view(buffer_, buffer_.begin(), buffer_.end());
    // else
    // content_ = string_view(buffer_, content_.begin(), buffer_.end());

    // buffer_.erase_head_until(buffer_.end());
    // if (content_.length() > content_length_)
    // content_.erase_tail(content_.length() - content_length_);

    // if (content_.length() == content_length_)
    // parse_state_ = ParseState::DONE;

    // std::cout << "CL in header vs parsed: " << content_length_ << " : "
    // << content_.length() << std::endl;

    // // std::cout << string_to_hex(content_.str()) << std::endl;

    // return;
    // }

    string_view header(buffer_);
    do
    {
        auto first_nl_pos = buffer_.find('\n');
        if (first_nl_pos == buffer_.end())
            return;

        auto line = string_view{buffer_, buffer_.begin(), first_nl_pos};
        // std::cout << line.str() << std::endl;
        strip_cr(line);
        buffer_.erase_head_until(std::next(first_nl_pos));

        if (parse_state_ == ParseState::PARSING_REQUEST_LINE)
        {
            request_line_ = line;
            parse_state_ = ParseState::PARSING_HEADERS;
        }
        else if (parse_state_ == ParseState::PARSING_HEADERS)
        {
            using namespace str;

            if (line.empty())
            {
                static const std::string content_length_str = "Content-Length";
                std::string l = headers_[content_length_str];
                parse_state_ = ParseState::DONE;
                if (!l.empty())
                {
                    content_length_ = std::stoi(l);
                    content_ =
                        string_view(buffer_, buffer_.begin(), buffer_.end());
                    break;
                }
            }
            else if (line.starts_with(' ') || line.starts_with('\t'))
                add_line_to_multiline_header(line, header);
            else
            {
                string_view header_data(buffer_);
                std::tie(header, header_data) = line.split_first(':');
                if (!header.empty() && !header_data.empty())
                    headers_[header.str()] = trim_copy(header_data).str();
            }
        }
    } while (!buffer_.empty());
}

void Request::add_line_to_multiline_header(const string_view& line,
                                           const string_view& header)
{
    auto non_whitespace_pos = line.find_first_not_of("\t ");
    if (non_whitespace_pos != line.end())
        if (!header.empty())
            headers_[header.str()] += line.substr(non_whitespace_pos).str();
}
}
