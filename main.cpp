#include <server_socket.h>
#include <sys/socket.h>
#include <unistd.h>

#include <utility>

class ChildThread : public Callable
{
public:
    virtual void operator()(int client_fd) override
    {
        if (send(client_fd, "Hello, world!", 13, 0) == -1)
            return;

        close(client_fd);
    }
};

int main()
{
    ChildThread c;
    ServerSocket s;
    s.bind();
    s.listen();
    s.accept(std::move(c));

    return 0;
}
