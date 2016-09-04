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
    using string_view = str::string_view<str::character_array>;
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
    Request(str::character_array& buffer);
    ~Request() = default;
    Request(const Request&) = delete;
    Request(Request&&) = default;

    explicit operator bool() const { return is_done(); }
    std::string operator[](const std::string& header) const;
    std::string data() const { return buffer_.data(); }
    std::string request_line() const { return request_line_.str(); }
    std::size_t content_length() const { return content_length_; }

    std::string content() const { return content_.str(); }

    void process_buffer();
    bool is_done() const { return parse_state_ == ParseState::DONE; }
    bool is_failed() const
    {
        return parse_state_ == ParseState::PARSING_FAILED;
    }

private:
    void add_line_to_multiline_header(const string_view& line,
                                      const string_view& header);

private:
    RequestType request_type_;
    ParseState parse_state_;
    str::character_array& buffer_;
    string_view request_line_;
    string_view content_;
    std::size_t content_length_;
    std::unordered_map<std::string, std::string> headers_;
};
}
