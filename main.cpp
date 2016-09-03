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
        mystr::MyStringBuffer buffer;
        auto message = http::receive_request(client_fd, buffer);
        if (message)
        {
            // std::cout << message.request_line() << std::endl;
            // std::cout << message["Host"] << std::endl;
            // std::cout << message["User-Agent"] << std::endl;

            // std::cout << "Data:\n" << message.data() << std::endl;
            const std::string& host = message["Host"];
            ClientSocket client;
            if (!client.connect(host, 80))
                std::cout << "failed to connect" << std::endl;
            else
            {
                std::cout << "sending to " << host << ": " << message.data()
                          << std::endl;
                auto sent = util::socket::send(client.sockfd_, message.data());
                std::cout << "sent bytes: " << sent << std::endl;

                std::string buf;
                buf.reserve(50000);
                buf = util::socket::recv(client.sockfd_);
                std::cout << "got " << buf << std::endl;

                util::socket::send(client_fd, buf);
                // auto response = http::receive_response(client.sockfd_);
                // std::cout << "waiting for response" << std::endl;
                // if (response)
                // {
                // std::cout << "response: " << std::endl;
                // // std::cout << response->data() << std::endl;
                // }
            }
        }

        close(client_fd);
    }
};

int main()
{
    Server<ChildThread> s;
    s.start(4999);
    return 0;
}
