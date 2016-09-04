#include <http_message.hpp>

#include <sys/socket.h>
#include <iostream>

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
Message::Message(int socket_fd)
    : receiver_(socket_fd)
    , parse_state_(ParseState::PARSING)
{
}
void Message::receive()
{
    receive_start_line();
    receive_headers();
}

void Message::receive_start_line()
{
    do
    {
        start_line_ = readline();
        while (start_line_.starts_with("\r\n"))
            start_line_.erase_head(2);
    } while (!start_line_.is_empty());
}

void Message::receive_headers()
{
    string_view line, header, header_data;
    do
    {
        line = readline();
        std::tie(header, header_data) = line.split_first(':');
        if (!header.is_empty() && !header_data.is_empty())
            headers_[header.str()] = trim_copy(header_data).str();
    } while (!line.is_empty());
    parse_state_ = ParseState::DONE;
}

Message::string_view Message::readline()
{
    auto it = buffer_.find('\n');
    while (it == buffer_.end())
    {
        receiver_.receive(buffer_);
        it = buffer_.find('\n');
    }

    auto line = string_view(buffer_, buffer_.begin(), it);
    strip_cr(line);

    buffer_.erase_head_until(std::next(it));
    return line;
}

std::string Message::operator[](const std::string& header_string) const
{
    auto it = headers_.find(header_string);
    if (it != headers_.end())
        return it->second;
    else
        return {};
}
}
