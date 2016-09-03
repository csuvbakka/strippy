#include <socket_functions.hpp>

namespace util
{
namespace socket
{

template <> std::size_t send(int socket_fd, const std::string& message)
{
    return ::send(socket_fd, message.c_str(), message.length(), 0);
}

template <typename T> std::size_t send(int socket_fd, const T* message);
template <> std::size_t send(int socket_fd, const char* message)
{
    return ::send(socket_fd, message, strlen(message), 0);
}

std::string recv(int socket_fd, int flags)
{
    char buf[1024] = {};
    std::size_t bytes = ::recv(socket_fd, buf, 1024, flags);
    if (bytes <= 0)
        return {};
    else
        return std::string(buf).substr(0, bytes);
}

void recv(int socket_fd, mystr::MyStringBuffer& buffer)
{
    char buf[1024] = {};
    std::size_t bytes = ::recv(socket_fd, buf, 1024, MSG_DONTWAIT);
    if (bytes > 0)
        buffer += buf;
}

std::string recv_no_wait(int socket_fd)
{
    return recv(socket_fd, MSG_DONTWAIT);
}
}
}
