#pragma once

#include <sys/types.h>
#include <unordered_map>
#include <my_string.hpp>
#include <receive_buffer.hpp>

namespace http
{
class Message
{
    using string_buffer = mystr::MyStringBuffer;
    using string_view = mystr::MyString;
    using receive_buffer = util::socket::ReceiveBuffer<string_buffer, 1024>;

    enum class ParseState
    {
        PARSING,
        DONE,
        FAILED
    };

public:
    Message(int socket_fd);

    void receive();
    std::size_t content_length() const { return content_length_; }

    explicit operator bool() const { return parse_state_ == ParseState::DONE; }
    std::string operator[](const std::string& header) const;

private:
    string_view readline();
    void receive_start_line();
    void receive_headers();
    void receive_content();

private:
    string_buffer buffer_;
    receive_buffer receive_buffer_;
    string_view start_line_;
    ParseState parse_state_;
    std::unordered_map<std::string, std::string> headers_;
    std::size_t content_length_;
};
}
