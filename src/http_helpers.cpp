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
int receive_request(int client_fd, mystr::MyStringBuffer& buffer,
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

int receive_response(int client_fd, mystr::MyStringBuffer& buffer,
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

// std::experimental::optional<http::Response> receive_response(int client_fd)
// {
// std::string message = {};
// std::size_t content_length = 0, headers_length;
// while (1)
// {
// message += util::socket::recv(client_fd);
// if (content_length > 0)
// {
// std::cout << "until: " << content_length + headers_length
// << std::endl;
// std::cout << "actual: " << message.length() << std::endl;
// std::cout << message << std::endl;
// if (message.length() >= headers_length + content_length)
// break;
// }
// else
// {
// auto headers_end_pos = message.find("\r\n\r\n");
// if (headers_end_pos != std::string::npos)
// {
// headers_length = headers_end_pos + 4;
// const std::string to_find = "Content-Length:";
// auto pos = message.find(to_find);
// if (pos != std::string::npos)
// {
// std::cout << message.substr(pos, to_find.length() + 10)
// << std::endl;
// auto start = pos + to_find.length();
// auto end = message.find("\r\n", start);
// content_length = std::stoi(util::string::ltrim(
// message.substr(start, end - start)));
// std::cout << "content length: " << content_length
// << std::endl;
// message.reserve(content_length + headers_length);
// }
// }
// }
// std::this_thread::sleep_for(std::chrono::milliseconds(1));
// }
// std::cout << "headers length: " << headers_length << std::endl;
// std::cout << "content length: " << content_length << std::endl;

// return http::Response(message);
// }

// std::experimental::optional<http::Response> receive_response(int client_fd)
// {
// std::string message = {}, buffer = {};
// buffer.reserve(1024);
// message.reserve(1024);
// do
// {
// std::cout << "filling buffer" << std::endl;
// buffer = util::socket::recv(client_fd);
// std::cout << "got " << buffer << std::endl;
// message += buffer;
// } while (!buffer.empty() && !util::string::ends_with(buffer, "\r\n\r\n"));
// // std::cout << message << std::endl;

// return http::Response(message);
// }
}
