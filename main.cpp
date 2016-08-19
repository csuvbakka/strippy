#include <server.h>
#include <socket_functions.h>
// #include <sys/socket.h>
#include <unistd.h>

#include <utility>
#include <functional>
#include <iostream>

struct ChildThread
{
    void operator()(int client_fd)
    {
        send(client_fd, "Hello bello");
        send(client_fd, 1337);
        send(client_fd, std::string("almafa"));

        close(client_fd);
    }
};

int main()
{
    Server<ChildThread> s;
    s.start(4999);
    return 0;
}
