#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <my_string.hpp>
#include <iostream>

namespace util
{
namespace socket
{
template <typename buffer_to_hold_data, int receive_buffer_size> class Receiver
{

public:
    Receiver(int socket_fd)
        : socket_fd_(socket_fd)
    {
    }
    ssize_t receive(buffer_to_hold_data& buffer)
    {
        ssize_t bytes;
        do
        {
            bytes = ::recv(socket_fd_, recv_buffer_, receive_buffer_size,
                           MSG_DONTWAIT);
        } while (bytes < 0);

        buffer.append(recv_buffer_, bytes);

        std::cout << buffer.str() << std::endl;

        return bytes;
    }

private:
    int socket_fd_;
    char recv_buffer_[receive_buffer_size];
};
}
}
