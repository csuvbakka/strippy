#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <my_string.hpp>
#include <iostream>
#include <chrono>

namespace util
{
namespace socket
{
template <typename _string_type_, int receive_buffer_size> class Receiver
{

public:
    typedef _string_type_ string_type;
    Receiver(int socket_fd)
        : socket_fd_(socket_fd)
    {
    }
    ssize_t receive(string_type& buffer)
    {
        ssize_t bytes;
        do
        {
            bytes = ::recv(socket_fd_, recv_buffer_, receive_buffer_size,
                           MSG_DONTWAIT);
            if (bytes < 0)
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } while (bytes < 0);

        buffer.append(recv_buffer_, bytes);

        // std::cout << buffer << std::endl;

        return bytes;
    }

private:
    int socket_fd_;
    char recv_buffer_[receive_buffer_size];
};
}
}
