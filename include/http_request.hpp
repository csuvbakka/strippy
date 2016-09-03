#pragma once

// #include <functional>
#include <string>
#include <unordered_map>

#include <my_string.hpp>

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
    Request(mystr::MyStringBuffer& buffer);

    std::string request_line() { return request_line_.str(); }
    std::string operator[](const std::string& header);

    void process_buffer();
    bool done_parsing() { return parse_state_ == ParseState::DONE; }

private:
    void add_line_to_multiline_header(const mystr::MyString& line,
                                      const mystr::MyString& header);

private:
    RequestType request_type_;
    ParseState parse_state_;
    mystr::MyStringBuffer& buffer_;
    mystr::MyString request_line_;
    std::unordered_map<std::string, std::string> headers_;
};
}
