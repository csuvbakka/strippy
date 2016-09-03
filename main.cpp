#include <server.hpp>
#include <socket_functions.hpp>

#include <unistd.h>
#include <utility>
#include <functional>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <iterator>
#include <experimental/optional>

#include <http_request.hpp>
#include <http_response.hpp>
#include <timer.hpp>
#include <client_socket.hpp>
#include <string_utils.hpp>
#include <http_helpers.hpp>

struct ChildThread
{
    void operator()(int client_fd)
    {
        auto message = http::receive_request(client_fd);
        if (message)
        {
            std::cout << message->request_line() << std::endl;
            // std::cout << (*message)["Host"] << std::endl;
            // std::cout << (*message)["User-Agent"] << std::endl;
            // ClientSocket client;
            // if (!client.connect((*message)["Host"], 80))
            // std::cout << "failed to connect" << std::endl;
            // std::cout << "sending " << message->data() << std::endl;
            // auto sent = util::socket::send(client.sockfd_, message->data());
            // std::cout << "sent bytes: " << sent << std::endl;
            // auto response = http::receive_response(client.sockfd_);
            // std::cout << "waiting for response" << std::endl;
            // if (response)
            // {
            // std::cout << "response: " << std::endl;
            // // std::cout << response->data() << std::endl;
            // }
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
