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
Request::Request()
    : parse_state_(ParseState::NOT_STARTED)
    , buff_()
    , request_line_(buff_)
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
        while (buff_.starts_with("\r\n"))
            buff_.erase_head(2); // RFC2616 - 4.1
        parse_state_ = ParseState::PARSING_REQUEST_LINE;
    }

    mystr::MyString header(buff_);
    do
    {
        auto first_nl_pos = buff_.find_first_of('\n');
        if (first_nl_pos == buff_.end())
            return;

        auto line = mystr::MyString{buff_, buff_.begin(), first_nl_pos};
        strip_cr(line);
        buff_.erase_head(std::next(first_nl_pos));

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
                mystr::MyString header_data(buff_);
                std::tie(header, header_data) = line.split_first(':');
                if (!header.is_empty() && !header_data.is_empty())
                    headers_[header.str()] = trim_copy(header_data).str();
            }
        }
    } while (!buff_.is_empty());
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

void Request::add_line_to_multiline_header(const mystr::MyString& line,
                                           const mystr::MyString& header)
{
    auto non_whitespace_pos = line.find_first_not_of("\t ");
    if (non_whitespace_pos != line.end())
        if (!header.is_empty())
            headers_[header.str()] += line.substr(non_whitespace_pos).str();
}
// void Request::add_line_to_multiline_header(const std::string& line,
// const std::string& header)
// {
// auto non_whitespace_pos = line.find_first_not_of("\t ");
// if (non_whitespace_pos != std::string::npos)
// if (!header.empty())
// headers_[header] += line.substr(non_whitespace_pos);
// }

// Request& operator>>(Request& lhs, const std::string& rhs)
// {
// lhs.buffer_ += rhs;
// lhs.process_buffer();
// return lhs;
// }
Request& operator>>(Request& lhs, const std::string& rhs)
{
    lhs.buff_ += rhs;
    lhs.process_buffer();
    return lhs;
}
}
