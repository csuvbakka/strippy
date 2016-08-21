#pragma once

#include <sys/socket.h>
#include <string>
#include <cstring>

template <typename T> std::size_t send(int socket_fd, const T& message)
{
    std::string message_str = std::to_string(message);
    return send(socket_fd, message_str);
}

template <> std::size_t send(int socket_fd, const std::string& message)
{
    return ::send(socket_fd, message.c_str(), message.length(), 0);
}

template <typename T> std::size_t send(int socket_fd, const T* message);
template <> std::size_t send(int socket_fd, const char* message)
{
    return ::send(socket_fd, message, strlen(message), 0);
}

std::string recv(int socket_fd)
{
    char buf[1024] = {};
    std::size_t bytes = ::recv(socket_fd, buf, 1024, MSG_DONTWAIT);
    if (bytes <= 0)
        return {};
    else
        return std::string(buf).substr(0, bytes);
}
