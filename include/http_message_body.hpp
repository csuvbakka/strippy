#pragma once

#include <http_message.hpp>

namespace http
{
template <typename buffer_reader> class MessageBody
{
public:
    template <typename T>
    MessageBody(const Message<T>& message, buffer_reader& r)
        : buffer_reader_(r)
        , bytes_received_(0)
    {
        const auto& content_length_header = message["Content-Length"];
        if (!content_length_header.empty())
            content_length_ = std::stoi(content_length_header);
    }

    void receive()
    {
        if (content_length_ == 0)
            return;

        ssize_t bytes;
        do
        {
            bytes = buffer_reader_.read_chunk();
            bytes_received_ += bytes;
        } while (bytes_received_ < content_length_);
    }

    explicit operator bool() const
    {
        return bytes_received_ >= content_length_;
    }

    long int content_length() const { return content_length_; }

private:
    buffer_reader buffer_reader_;
    long int content_length_;
    long int bytes_received_;
};
}
