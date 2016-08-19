#include <server.h>
#include <socket_functions.h>
// #include <sys/socket.h>
#include <unistd.h>

#include <utility>
#include <functional>
#include <iostream>
#include <algorithm>

namespace
{
bool ends_with(const std::string& value, const std::string& ending)
{
    if (ending.size() > value.size())
        return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

std::string receive_HTTP(int client_fd)
{
    std::size_t bytes;
    std::string buffer = {}, message = {};
    do
    {
        std::tie(bytes, buffer) = recv(client_fd);
        message += buffer;
        std::cout << buffer << std::endl;
    } while (!ends_with(message, "\r\n\r\n"));

    return message;
}
}

struct ChildThread
{
    void operator()(int client_fd)
    {
        std::size_t bytes;
        std::string buffer, message = {};
        std::cout << "receiving\n";

        message = receive_HTTP(client_fd);

        std::cout << message << std::endl;
        // send(client_fd, "Content-Type: text/plain\r\n");
        // send(client_fd, 1337);
        // send(client_fd, std::string("almafa"));

        close(client_fd);
    }
};

int main()
{
    Server<ChildThread> s;
    s.start(4999);
    return 0;
}
