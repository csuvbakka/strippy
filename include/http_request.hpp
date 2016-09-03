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
        PARSING_CONTENT,
        DONE,
        PARSING_FAILED
    };

public:
    Request(mystr::MyStringBuffer& buffer);
    ~Request() = default;
    Request(const Request&) = delete;
    Request(Request&&) = default;

    explicit operator bool() const { return is_done(); }
    std::string operator[](const std::string& header) const;
    std::string data() const { return buffer_.data(); }
    std::string request_line() const { return request_line_.str(); }
    std::string content() const { return content_.str(); }

    void process_buffer();
    bool is_done() const { return parse_state_ == ParseState::DONE; }
    bool is_failed() const
    {
        return parse_state_ == ParseState::PARSING_FAILED;
    }

private:
    void add_line_to_multiline_header(const mystr::MyString& line,
                                      const mystr::MyString& header);

private:
    RequestType request_type_;
    ParseState parse_state_;
    mystr::MyStringBuffer& buffer_;
    mystr::MyString request_line_;
    mystr::MyString content_;
    int content_length_;
    std::unordered_map<std::string, std::string> headers_;
};
}
