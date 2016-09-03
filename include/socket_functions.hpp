#pragma once

#include <sys/socket.h>
#include <string>
#include <cstring>
#include <my_string.hpp>

namespace util
{
namespace socket
{
template <typename T> std::size_t send(int socket_fd, const T& message)
{
    std::string message_str = std::to_string(message);
    return send(socket_fd, message_str);
}

template <> std::size_t send(int socket_fd, const std::string& message);

template <typename T> std::size_t send(int socket_fd, const T* message);
template <> std::size_t send(int socket_fd, const char* message);

std::string recv(int socket_fd, int flags = 0);
void recv(int socket_fd, mystr::MyStringBuffer& buffer);
std::string recv_no_wait(int socket_fd);
}
}
