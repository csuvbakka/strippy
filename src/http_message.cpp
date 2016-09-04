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
    : receive_buffer_(socket_fd, buffer_)
    , parse_state_(ParseState::PARSING)
{
}
void Message::receive()
{
    receive_start_line();
    receive_headers();
    // receive_content();
}

void Message::receive_start_line()
{
    start_line_ = readline();
    // std::cout << start_line_.str() << std::endl;
}

void Message::receive_headers()
{
    string_view line, header, header_data;
    do
    {
        line = readline();
        // std::cout << line.str() << std::endl;
        std::tie(header, header_data) = line.split_first(':');
        if (!header.is_empty() && !header_data.is_empty())
            headers_[header.str()] = trim_copy(header_data).str();
    } while (!line.is_empty());
    parse_state_ = ParseState::DONE;
}

void Message::receive_content()
{
    static const std::string ContentLength = "Content-Length";
    auto length = headers_[ContentLength];
    if (!length.empty())
    {
        content_length_ = std::stoi(length);

        // remove remainder from buffer_
        // read content
    }
}

Message::string_view Message::readline()
{
    auto it = buffer_.find('\n');
    while (it == buffer_.end())
    {
        receive_buffer_.receive();
        it = buffer_.find('\n');
    }

    auto line = string_view(buffer_, buffer_.begin(), it);
    buffer_.erase_head_until(std::next(it));
    strip_cr(line);
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
