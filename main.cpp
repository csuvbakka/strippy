#include <server.h>
#include <sys/socket.h>
#include <unistd.h>

#include <utility>
#include <functional>
#include <iostream>

struct ChildThread
{
    void operator()(int client_fd)
    {
        if (send(client_fd, "Hello, world!", 13, 0) == -1)
            return;

        close(client_fd);
    }
};

int main()
{
    Server<ChildThread> s;
    s.start(4999);
    return 0;
}
