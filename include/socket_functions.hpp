#pragma once

#include <sys/socket.h>
#include <string>
#include <cstring>
#include <my_string.hpp>

namespace util
{
namespace socket
{

std::size_t send(int socket_fd, const std::string& message);
std::size_t send(int socket_fd, const char* message, std::size_t len);

int recvv(int socket_fd, char* buffer, std::size_t len);
// std::string recv(int socket_fd, int flags = 0);
int recv(int socket_fd, str::character_array& buffer);
void recv_dont_wait(int socket_fd, str::character_array& buffer);
}
}
