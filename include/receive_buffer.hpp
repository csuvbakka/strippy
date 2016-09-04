#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <my_string.hpp>
#include <iostream>

namespace util
{
namespace socket
{
template <typename _buffer_type_, int receive_buffer_size> class Receiver
{

public:
    typedef _buffer_type_ buffer_type;
    Receiver(int socket_fd)
        : socket_fd_(socket_fd)
    {
    }
    ssize_t receive(buffer_type& buffer)
    {
        ssize_t bytes;
        do
        {
            bytes = ::recv(socket_fd_, recv_buffer_, receive_buffer_size,
                           MSG_DONTWAIT);
        } while (bytes < 0);

        buffer.append(recv_buffer_, bytes);

        std::cout << buffer << std::endl;

        return bytes;
    }

private:
    int socket_fd_;
    char recv_buffer_[receive_buffer_size];
};
}
}
