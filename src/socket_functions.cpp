#include <socket_functions.hpp>
#include <iostream>

namespace util
{
namespace socket
{

std::size_t send(int socket_fd, const std::string& message)
{
    return ::send(socket_fd, message.c_str(), message.length(), 0);
}

std::size_t send(int socket_fd, const char* message, std::size_t len)
{
    // return ::send(socket_fd, message, len, MSG_DONTWAIT);
    return ::send(socket_fd, message, len, 0);
}

int recvv(int socket_fd, char* buffer, std::size_t len)
{
    return ::recv(socket_fd, buffer, len, MSG_DONTWAIT);
}

int recv(int socket_fd, str::character_array& buffer)
{
    char buf[1024] = {};
    auto bytes = ::recv(socket_fd, buf, 1024, MSG_DONTWAIT);
    if (bytes > 0)
        buffer.append(buf, bytes);

    return bytes;
}

void recv_dont_wait(int socket_fd, str::character_array& buffer)
{
    char buf[1024] = {};
    std::size_t bytes = ::recv(socket_fd, buf, 1024, MSG_DONTWAIT);
    if (bytes > 0)
        buffer += buf;
}
}
}
