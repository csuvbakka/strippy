#pragma once

#include <string>
#include <unordered_map>

namespace http
{
enum class RequestType
{
    GET,
    POST,
    OTHER
};

class Request
{
    enum class ParseState
    {
        NOT_STARTED,
        PARSING_REQUEST_LINE,
        PARSING_HEADERS,
        PARSING_DATA,
        DONE
    };

public:
    Request();
    Request(const std::string& data);

    friend Request& operator>>(Request& lhs, const std::string& rhs);

    std::string data() { return data_; }
    std::string request_line() { return request_line_; }
    std::string operator[](const std::string& header);

    bool done_parsing() { return parse_state_ == ParseState::DONE; }

private:
    void process_buffer();

private:
    std::string data_;
    RequestType request_type_;
    std::unordered_map<std::string, std::string> headers_;
    std::string request_line_;

    std::string buffer_;
    ParseState parse_state_;
};

Request& operator>>(Request& lhs, const std::string& rhs);
}
