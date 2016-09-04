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
#include <http_message.hpp>
#include <buffer_reader.hpp>
#include <receive_buffer.hpp>

namespace
{
std::string string_to_hex(const std::string& input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();

    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
        output.push_back(' ');
    }
    return output;
}
}

// struct ChildThread
// {
// void operator()(int client_fd)
// {
// while (1)
// {
// str::character_array buffer;
// http::Request message(buffer);
// auto bytes = http::receive_request(client_fd, buffer, message);
// if (bytes == 0)
// break;

// std::cout << client_fd << " Victim => Proxy | "
// << message.request_line() << std::endl;

// const std::string& host = message["Host"];
// ClientSocket client;
// if (!client.connect(host, 80))
// std::cout << "failed to connect" << std::endl;
// else
// {
// // std::cout << "sending to " << host << ": " <<
// // message.data()
// // << std::endl;
// auto sent = util::socket::send(client.sockfd_, message.data());
// std::cout << client_fd << " Proxy => Host | " << sent
// << std::endl;

// str::character_array recv_buffer;
// http::Request response(recv_buffer);
// http::receive_request(client.sockfd_, recv_buffer, response);
// if (response && response.content_length() > 0)
// {
// util::socket::send(client_fd, response.data());
// const auto content_length = response.content_length();
// const auto buf_len = 1048576;
// // const auto buf_len = 2048;
// char buf[buf_len] = {};
// std::size_t sum = 0;
// if (!response.content().empty())
// sum += response.content().length();

// do
// {
// bytes =
// util::socket::recvv(client.sockfd_, buf, buf_len);
// if (bytes > 0)
// {

// std::cout << client_fd << " Proxy <= Host | ";
// std::cout << bytes << std::endl;
// sent = util::socket::send(client_fd, buf, bytes);
// std::cout << client_fd << "Victim <= Proxy | "
// << sent << std::endl;
// sum += sent;
// }
// std::this_thread::sleep_for(
// std::chrono::milliseconds(1));
// } while (bytes != 0 && !(sum >= content_length));
// }
// }

// if (message["Connection"] == "close")
// break;

// std::this_thread::sleep_for(std::chrono::milliseconds(1));
// }

// std::cout << client_fd << " Closing " << std::endl;
// close(client_fd);
// }
// };
struct ChildThread
{
    void operator()(int client_fd)
    {
        while (1)
        {
            // using string_buffer = str::character_array;
            using string_buffer = std::string;
            using receiver = util::socket::Receiver<string_buffer, 1024>;
            using buffer_reader = str::BufferReader<receiver>;

            string_buffer buffer;
            buffer_reader reader(client_fd, buffer);

            http::Message<buffer_reader> message(reader);
            message.receive();
            if (message)
            {
                if (buffer.empty())
                    std::cout << "Buffer empty" << std::endl;
                else
                    std::cout << "Left in buffer: " << buffer << std::endl;
                break;
            }
        }
    }
};

int main()
{
    Server<ChildThread> s;
    s.start(4999);
    return 0;
}
