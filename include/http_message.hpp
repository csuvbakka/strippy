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
    using receiver = util::socket::Receiver<string_buffer, 1024>;

    enum class ParseState
    {
        PARSING,
        DONE,
        FAILED
    };

public:
    Message(int socket_fd);

    void receive();

    explicit operator bool() const { return parse_state_ == ParseState::DONE; }
    std::string operator[](const std::string& header) const;

private:
    string_view readline();
    void receive_start_line();
    void receive_headers();

private:
    string_buffer buffer_;
    receiver receiver_;
    ParseState parse_state_;

    string_view start_line_;
    std::unordered_map<std::string, std::string> headers_;
};
}
