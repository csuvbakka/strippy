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
        , bytes_received_(r.buffer_size())
    {
        const auto& content_length_header = message["Content-Length"];
        if (!content_length_header.empty())
            content_length_ = std::stoi(content_length_header);
        else
            content_length_ = 0;
    }

    void receive()
    {
        if (content_length_ == 0 || bytes_received_ >= content_length_)
        {
            std::cout << "Already have it in buffer" << std::endl;
            return;
        }

        ssize_t bytes;
        do
        {
            bytes = buffer_reader_.read_chunk();
            bytes_received_ += bytes;
            std::cout << "bytes_received_: " << bytes_received_ << std::endl;
        } while (bytes_received_ < content_length_ && bytes != 0);
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
