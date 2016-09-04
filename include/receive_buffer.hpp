#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <my_string.hpp>
#include <iostream>

namespace util
{
namespace socket
{
template <typename buffer_to_hold_data, int receive_buffer_size>
class ReceiveBuffer
{

public:
    ReceiveBuffer(int socket_fd, buffer_to_hold_data& buffer)
        : socket_fd_(socket_fd)
        , buffer_(buffer)
    {
    }
    ssize_t receive()
    {
        ssize_t bytes;
        do
        {
            bytes = ::recv(socket_fd_, recv_buffer_, receive_buffer_size,
                           MSG_DONTWAIT);
        } while (bytes < 0);

        buffer_.append(recv_buffer_, bytes);

        std::cout << buffer_.str() << std::endl;

        return bytes;
    }

    buffer_to_hold_data& buffer() { return buffer_; }
private:
    int socket_fd_;
    char recv_buffer_[receive_buffer_size];
    buffer_to_hold_data buffer_;
};
}
}
