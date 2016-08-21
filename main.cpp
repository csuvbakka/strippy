#include <server.h>
#include <socket_functions.h>
#include <unistd.h>

#include <utility>
#include <functional>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <iterator>
#include <experimental/optional>

#include <http.h>
#include <timer.h>

namespace
{
bool ends_with(const std::string& value, const std::string& ending)
{
    if (ending.size() > value.size())
        return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

std::experimental::optional<HttpRequest> receive_HTTP(int client_fd)
{
    std::string message = {};
    Timer timer(5000);
    do
    {
        message += recv(client_fd);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        timer.reset();
    } while (!ends_with(message, "\r\n\r\n") && !timer.is_timed_out());

    if (timer.is_timed_out())
        return {};
    else
        return HttpRequest(message);
}
}

struct ChildThread
{
    void operator()(int client_fd)
    {
        auto message = receive_HTTP(client_fd);
        if (message)
        {
            std::cout << message->request_line() << std::endl;
            std::cout << (*message)["Host"] << std::endl;
            std::cout << (*message)["User-Agent"] << std::endl;
        }

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
