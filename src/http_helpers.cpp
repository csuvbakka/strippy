#include <http_helpers.hpp>
#include <timer.hpp>
#include <socket_functions.hpp>
#include <string_utils.hpp>
#include <my_string.hpp>

#include <iostream>
#include <thread>
#include <chrono>

namespace http
{
int receive_request(int client_fd, str::character_array& buffer,
                    http::Request& request)
{
    // Timer timer(5000);
    int bytes;
    // http::Request request(buffer);
    do
    {
        // util::socket::recv_dont_wait(client_fd, buffer);
        bytes = util::socket::recv(client_fd, buffer);
        if (bytes > 0)
            request.process_buffer();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        // timer.reset();
    } while (!request.is_done() && bytes != 0);
    // } while (!request.is_done() && !timer.is_timed_out() && bytes != 0);

    return bytes;
}

int receive_response(int client_fd, str::character_array& buffer,
                     http::Request& response)
{
    http::Request request(buffer);
    int bytes;
    do
    {
        bytes = util::socket::recv(client_fd, buffer);
        if (bytes > 0)
            request.process_buffer();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    } while (!request.is_done() && bytes != 0);

    return bytes;
}
}
