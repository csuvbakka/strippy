#pragma once

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
    Request();
    Request(const std::string& data);

    friend Request& operator>>(Request& lhs, const std::string& rhs);

    std::string data() { return data_; }
    std::string request_line() { return request_line_.str(); }
    std::string operator[](const std::string& header);

    bool done_parsing() { return parse_state_ == ParseState::DONE; }

private:
    void process_buffer();
    std::string get_headers_to_process();
    void add_line_to_multiline_header(const mystr::MyString& line,
                                      const mystr::MyString& header);

private:
    std::string data_;
    RequestType request_type_;
    ParseState parse_state_;
    mystr::MyStringBuffer buff_;
    std::unordered_map<std::string, std::string> headers_;
    mystr::MyString request_line_;

    std::string buffer_;
};

Request& operator>>(Request& lhs, const std::string& rhs);
}
