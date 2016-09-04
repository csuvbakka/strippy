#pragma once

#include <sys/types.h>
#include <unordered_map>
#include <my_string.hpp>

namespace http
{
template <typename buffer_reader> class Message
{
    using string_view = typename buffer_reader::string_view;

    enum class ParseState
    {
        PARSING,
        DONE,
        FAILED
    };

public:
    Message(buffer_reader& r)
        : buffer_reader_(r)
        , parse_state_(ParseState::PARSING)
    {
    }

    void receive()
    {
        receive_start_line();
        receive_headers();
    }

    explicit operator bool() const { return parse_state_ == ParseState::DONE; }
    std::string operator[](const std::string& header_string) const
    {
        auto it = headers_.find(header_string);
        if (it != headers_.end())
            return it->second;
        else
            return {};
    }

private:
    void receive_start_line()
    {
        do
        {
            start_line_ = buffer_reader_.readline();
            while (start_line_.starts_with("\r\n"))
                start_line_.erase_head(2);
        } while (!start_line_.empty());
    }
    void receive_headers()
    {
        string_view line, header, header_data;
        do
        {
            line = buffer_reader_.readline();
            std::tie(header, header_data) = line.split_first(':');
            if (!header.empty() && !header_data.empty())
                headers_[header.str()] = trim_copy(header_data).str();
        } while (!line.empty());
        parse_state_ = ParseState::DONE;
    }

private:
    buffer_reader buffer_reader_;
    ParseState parse_state_;

    string_view start_line_;
    std::unordered_map<std::string, std::string> headers_;
};
}
